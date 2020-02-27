/*
 * Simple tree-walk code generator to produce LLVM assembly for the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#include "LLVMCodeGen.h"

Module* LLVMCodeGen::getModule() const {
    return module_.get();
}

void LLVMCodeGen::visit(ModuleNode &node) {
    module_ = std::make_unique<Module>(node.getName(), context_);
    for (size_t i = 0; i < node.getVariableCount(); i++) {
        auto variable = node.getVariable(i);
        auto type = getLLVMType(variable->getType());
        auto value = new GlobalVariable(*module_.get(), type, false,
                GlobalValue::CommonLinkage, Constant::getNullValue(type), variable->getName());
        // TODO: set alignment
        // value->setAlignment(variable->getType()->getSize());
        values_[variable] = value;
    }
    // generate code for procedures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
    // generate code for main
    auto main = module_->getOrInsertFunction("main", getLLVMType(BasicTypeNode::INTEGER));
    function_ = cast<Function>(main.getCallee());
    auto entry = BasicBlock::Create(context_, "entry", function_);
    builder_.SetInsertPoint(entry);
    level_ = node.getLevel() + 1;
    node.getStatements()->accept(*this);
    builder_.CreateRet(builder_.getInt32(0));
}

void LLVMCodeGen::visit(ProcedureNode &node) {
    std::vector<Type*> params;
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        auto param = node.getParameter(i);
        params.push_back(getLLVMType(param->getType(), param->isVar()));
    }
    auto type = FunctionType::get(getLLVMType(node.getReturnType()), params, node.hasVarArgs());
    auto proc = module_->getOrInsertFunction(node.getName(), type);
    if (!node.isExtern()) {
        auto func = cast<Function>(proc.getCallee());
        Function::arg_iterator args = func->arg_begin();
        for (size_t i = 0; i < node.getParameterCount(); i++) {
            auto arg = args++;
            auto param = node.getParameter(i);
            arg->setName(param->getName());
            values_[param] = arg;
        }
        function_ = cast<Function>(proc.getCallee());
        auto entry = BasicBlock::Create(context_, "entry", function_);
        builder_.SetInsertPoint(entry);
        for (size_t i = 0; i < node.getVariableCount(); i++) {
            auto var = node.getVariable(i);
            auto value = builder_.CreateAlloca(getLLVMType(var->getType()), 0, var->getName());
            values_[var] = value;
        }
        level_ = node.getLevel() + 1;
        node.getStatements()->accept(*this);
        if (node.getReturnType() == nullptr) {
            builder_.CreateRetVoid();
        }
        auto block = builder_.GetInsertBlock();
        if (block->getTerminator() == nullptr) {
            if (node.getReturnType() != nullptr && block->size() > 0) {
                logger_->error(node.getFilePos(), "function \"" + node.getName() + "\" has no return statement.");
            } else {
                builder_.CreateUnreachable();
            }
        }
    }
}

void LLVMCodeGen::visit(ReferenceNode &node) {
    auto ref = node.dereference();
    int level = ref->getLevel();
    if (level == 1) /* global level */ {
        value_ = values_[ref];
    } else if (level == level_) /* same procedure level */ {
        value_ = values_[ref];
    } else if (level > level_) /* parent procedure level */ {
        // todo global display?
        logger_->error(ref->getFilePos(), "Referencing variables of parent procedures is not yet supported.");
    } else /* error */ {
        logger_->error(ref->getFilePos(), "Cannot reference variable of child procedure.");
    }
    auto type = ref->getType();
    if (node.getSelectorCount() > 0) {
        auto selector_t = type;
        auto base = value_;
        if (ref->getNodeType() == NodeType::parameter) {
            auto param = dynamic_cast<ParameterNode*>(ref);
            // create a base address within the stack frame of the current procedure for
            // array and record parameters that are passed by value
            if (!param->isVar() && (selector_t->getNodeType() == NodeType::array_type ||
                                    selector_t->getNodeType() == NodeType::record_type)) {
                base = builder_.CreateAlloca(getLLVMType(type));
                builder_.CreateStore(value_, base);
            }
        }
        std::vector<Value*> indices;
        indices.push_back(builder_.getInt32(0));
        for (size_t i = 0; i < node.getSelectorCount(); i++) {
            if (selector_t->getNodeType() == NodeType::array_type) {
                // handle array index access
                setRefMode(true);
                node.getSelector(i)->accept(*this);
                indices.push_back(value_);
                restoreRefMode();
                selector_t = dynamic_cast<ArrayTypeNode*>(selector_t)->getMemberType();
            } else if (selector_t->getNodeType() == NodeType::record_type) {
                // handle record field access
                auto selector = node.getSelector(i);
                auto decl = dynamic_cast<ReferenceNode*>(selector)->dereference();
                auto field = dynamic_cast<FieldNode*>(decl);
                auto record_t = dynamic_cast<RecordTypeNode*>(selector_t);
                for (size_t pos = 0; pos < record_t->getFieldCount(); pos++) {
                    if (field == record_t->getField(pos)) {
                        indices.push_back(builder_.getInt32(pos));
                        break;
                    }
                }
                selector_t = selector->getType();
            }
        }
        value_ = builder_.CreateInBoundsGEP(getLLVMType(type), base, indices);
    }
    if (deref()) {
        if (ref->getNodeType() == NodeType::variable) {
            value_ = builder_.CreateLoad(value_);
        } else if (ref->getNodeType() == NodeType::parameter) {
            auto param = dynamic_cast<ParameterNode*>(ref);
            // load value of parameter that is either passed by reference or is an array or
            // record parameter since getelementptr only computes the address
            if (param->isVar() || param->getType()->getNodeType() == NodeType::array_type ||
                                  param->getType()->getNodeType() == NodeType::record_type) {
                value_ = builder_.CreateLoad(value_);
            }
        }
    }
}

void LLVMCodeGen::visit(ConstantDeclarationNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(FieldNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(ParameterNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(VariableDeclarationNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(BooleanLiteralNode &node) {
    value_ = node.getValue() ? builder_.getTrue() : builder_.getFalse();
}

void LLVMCodeGen::visit(IntegerLiteralNode &node) {
    value_ = builder_.getInt32(node.getValue());
}

void LLVMCodeGen::visit(StringLiteralNode &node) {
    std::string val = node.getValue();
    value_ = builder_.CreateGlobalStringPtr(val);
}

void LLVMCodeGen::visit(FunctionCallNode &node) {
    call(node);
}

void LLVMCodeGen::visit(UnaryExpressionNode &node) {
    node.getExpression()->accept(*this);
    switch (node.getOperator()) {
        case OperatorType::NOT: value_ = builder_.CreateNot(value_); break;
        case OperatorType::NEG: value_ = builder_.CreateNeg(value_); break;
    }
}

void LLVMCodeGen::visit(BinaryExpressionNode &node) {
    node.getLeftExpression()->accept(*this);
    auto lhs = value_;
    // Logical operators AND and OR are treated explicitly in order to enable short-circuiting.
    if (node.getOperator() == OperatorType::AND || node.getOperator() == OperatorType::OR) {
        // Create one block to evaluate the right-hand side and one to skip it.
        auto eval = BasicBlock::Create(context_, "eval", function_);
        auto skip = BasicBlock::Create(context_, "skip", function_);
        // Insert branch to decide whether to skip AND or OR
        if (node.getOperator() == OperatorType::AND) {
            builder_.CreateCondBr(value_, eval, skip);
        } else if (node.getOperator() == OperatorType::OR) {
            builder_.CreateCondBr(value_, skip, eval);
        }
        // Save the current (left-hand side) block.
        auto lhsBB = builder_.GetInsertBlock();
        // Create and populate the basic block to evaluate the right-hand side, if required.
        builder_.SetInsertPoint(eval);
        node.getRightExpression()->accept(*this);
        auto rhs = value_;
        if (node.getOperator() == OperatorType::AND) {
            value_ = builder_.CreateAnd(lhs, rhs);
        } else if (node.getOperator() == OperatorType::OR) {
            value_ = builder_.CreateOr(lhs, rhs);
        }
        // Save the current (right-hand side) block.
        auto rhsBB = builder_.GetInsertBlock();
        builder_.CreateBr(skip);
        // Create the basic block after the possible short-circuit,
        // use phi-value to "combine" value of both branches.
        builder_.SetInsertPoint(skip);
        auto phi = builder_.CreatePHI(value_->getType(), 2, "phi");
        phi->addIncoming(lhs, lhsBB);
        phi->addIncoming(value_, rhsBB);
        value_ = phi;
    } else {
        node.getRightExpression()->accept(*this);
        auto rhs = value_;
        switch (node.getOperator()) {
            case OperatorType::PLUS:
                value_ = builder_.CreateAdd(lhs, rhs);
                break;
            case OperatorType::MINUS:
                value_ = builder_.CreateSub(lhs, rhs);
                break;
            case OperatorType::TIMES:
                value_ = builder_.CreateMul(lhs, rhs);
                break;
            case OperatorType::DIV:
                value_ = builder_.CreateSDiv(lhs, rhs);
                break;
            case OperatorType::MOD:
                value_ = builder_.CreateSRem(lhs, rhs);
                break;
            case OperatorType::EQ:
                value_ = builder_.CreateICmpEQ(lhs, rhs);
                break;
            case OperatorType::NEQ:
                value_ = builder_.CreateICmpNE(lhs, rhs);
                break;
            case OperatorType::LT:
                value_ = builder_.CreateICmpSLT(lhs, rhs);
                break;
            case OperatorType::GT:
                value_ = builder_.CreateICmpSGT(lhs, rhs);
                break;
            case OperatorType::LEQ:
                value_ = builder_.CreateICmpSLE(lhs, rhs);
                break;
            case OperatorType::GEQ:
                value_ = builder_.CreateICmpSGE(lhs, rhs);
                break;
        }
    }
}

void LLVMCodeGen::visit(TypeDeclarationNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(ArrayTypeNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(BasicTypeNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(RecordTypeNode &node) { /* Node does not need code generation. */ }

void LLVMCodeGen::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        node.getStatement(i)->accept(*this);
    }
}

void LLVMCodeGen::visit(AssignmentNode &node) {
    setRefMode(true);
    node.getRvalue()->accept(*this);
    Value* rValue = value_;
    restoreRefMode();
    node.getLvalue()->accept(*this);
    Value* lValue = value_;
    value_ = builder_.CreateStore(rValue, lValue);
}

void LLVMCodeGen::visit(IfThenElseNode& node) {
    auto tail = BasicBlock::Create(context_, "tail", function_);
    auto if_true = BasicBlock::Create(context_, "if_true", function_);
    auto if_false = tail;
    if (node.hasElse() || node.hasElseIf()) {
        if_false = BasicBlock::Create(context_, "if_false", function_);
    }
    setRefMode(true);
    node.getCondition()->accept(*this);
    restoreRefMode();
    builder_.CreateCondBr(value_, if_true, if_false);
    builder_.SetInsertPoint(if_true);
    node.getThenStatements()->accept(*this);
    if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
        builder_.CreateBr(tail);
    }
    builder_.SetInsertPoint(if_false);
    for (size_t i = 0; i < node.getElseIfCount(); i++) {
        auto elsif = node.getElseIf(i);
        auto elsif_true = BasicBlock::Create(context_, "elsif_true", function_);
        auto elsif_false = tail;
        if (node.hasElse() || i + 1 < node.getElseIfCount()) {
            elsif_false = BasicBlock::Create(context_, "elsif_false", function_);
        }
        setRefMode(true);
        elsif->getCondition()->accept(*this);
        restoreRefMode();
        builder_.CreateCondBr(value_, elsif_true, elsif_false);
        builder_.SetInsertPoint(elsif_true);
        elsif->getStatements()->accept(*this);
        if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
            builder_.CreateBr(tail);
        }
        builder_.SetInsertPoint(elsif_false);
    }
    if (node.hasElse()) {
        node.getElseStatements()->accept(*this);
        if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
            builder_.CreateBr(tail);
        }
    }
    builder_.SetInsertPoint(tail);
}

void LLVMCodeGen::visit(ElseIfNode& node) { /* Code generated in the context of if-then-else node */ }

void LLVMCodeGen::visit(ProcedureCallNode& node) {
    call(node);
}

void LLVMCodeGen::visit(LoopNode& node) {
    // todo code generation for general loop
}

void LLVMCodeGen::visit(WhileLoopNode& node) {
    auto body = BasicBlock::Create(context_, "loop_body", function_);
    auto tail = BasicBlock::Create(context_, "tail", function_);
    setRefMode(true);
    node.getCondition()->accept(*this);
    restoreRefMode();
    builder_.CreateCondBr(value_, body, tail);
    builder_.SetInsertPoint(body);
    node.getStatements()->accept(*this);
    setRefMode(true);
    node.getCondition()->accept(*this);
    restoreRefMode();
    builder_.CreateCondBr(value_, body, tail);
    builder_.SetInsertPoint(tail);
}

void LLVMCodeGen::visit(RepeatLoopNode& node) {
    auto body = BasicBlock::Create(context_, "loop_body", function_);
    auto tail = BasicBlock::Create(context_, "tail", function_);
    builder_.CreateBr(body);
    builder_.SetInsertPoint(body);
    node.getStatements()->accept(*this);
    setRefMode(true);
    node.getCondition()->accept(*this);
    restoreRefMode();
    builder_.CreateCondBr(value_, tail, body);
    builder_.SetInsertPoint(tail);
}

void LLVMCodeGen::visit(ForLoopNode& node) {
    // initialize loop counter
    setRefMode(true);
    node.getLow()->accept(*this);
    auto start = value_;
    restoreRefMode();
    node.getCounter()->accept(*this);
    auto counter = value_;
    value_ = builder_.CreateStore(start, counter);
    // check whether to skip loop body
    setRefMode(true);
    node.getHigh()->accept(*this);
    auto end = value_;
    node.getCounter()->accept(*this);
    counter = value_;
    restoreRefMode();
    auto body = BasicBlock::Create(context_, "loop_body", function_);
    auto tail = BasicBlock::Create(context_, "tail", function_);
    if (node.getStep() > 0) {
        value_ = builder_.CreateICmpSLE(counter, end);
    } else {
        value_ = builder_.CreateICmpSGE(counter, end);
    }
    builder_.CreateCondBr(value_, body, tail);
    // loop body
    builder_.SetInsertPoint(body);
    node.getStatements()->accept(*this);
    // update loop counter
    setRefMode(true);
    node.getCounter()->accept(*this);
    restoreRefMode();
    counter = builder_.CreateAdd(value_, builder_.getInt32(node.getStep()));
    node.getCounter()->accept(*this);
    auto lValue = value_;
    builder_.CreateStore(counter, lValue);
    // check whether to exit loop body
    setRefMode(true);
    node.getHigh()->accept(*this);
    end = value_;
    node.getCounter()->accept(*this);
    counter = value_;
    restoreRefMode();
    if (node.getStep() > 0) {
        value_ = builder_.CreateICmpSGT(counter, end);
    } else {
        value_ = builder_.CreateICmpSLT(counter, end);
    }
    builder_.CreateCondBr(value_, tail, body);
    // after loop
    builder_.SetInsertPoint(tail);
}

void LLVMCodeGen::visit(ReturnNode& node) {
    setRefMode(true);
    node.getValue()->accept(*this);
    restoreRefMode();
    value_ = builder_.CreateRet(value_);
}

Type* LLVMCodeGen::getLLVMType(TypeNode *type, bool isPtr) {
    Type* result = nullptr;
    if (type == nullptr) {
        result = builder_.getVoidTy();
    } else if (types_[type] != nullptr) {
        result = types_[type];
    } else if (type->getNodeType() == NodeType::array_type) {
        auto array_t = dynamic_cast<ArrayTypeNode*>(type);
        result = ArrayType::get(getLLVMType(array_t->getMemberType()), array_t->getDimension());
    } else if (type->getNodeType() == NodeType::record_type) {
        std::vector<Type*> elem_ts;
        auto record_t = dynamic_cast<RecordTypeNode*>(type);
        for (size_t i = 0; i < record_t->getFieldCount(); i++) {
            elem_ts.push_back(getLLVMType(record_t->getField(i)->getType()));
        }
        result = StructType::create(elem_ts);
    } else if (type->getNodeType() == NodeType::basic_type) {
        if (type == BasicTypeNode::BOOLEAN) {
            result = builder_.getInt1Ty();
        } else if (type == BasicTypeNode::INTEGER) {
            result = builder_.getInt32Ty();
        } else if (type == BasicTypeNode::STRING) {
            result = builder_.getInt8PtrTy();
        }
    }
    if (result != nullptr) {
        types_[type] = result;
        if (isPtr) {
            result = PointerType::get(result, 0);
        }
    } else {
        logger_->error(type->getFilePos(), "Cannot map type to LLVM intermediate representation.");
    }
    return result;
}

void LLVMCodeGen::call(CallNode &node) {
    std::vector<Value*> params;
    std::string name = node.getProcedure()->getName();
    size_t fp_cnt = node.getProcedure()->getParameterCount();
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        setRefMode(i < fp_cnt ? !node.getProcedure()->getParameter(i)->isVar() : true);
        node.getParameter(i)->accept(*this);
        params.push_back(value_);
        restoreRefMode();
    }
    auto proc = module_->getFunction(name);
    if (proc) {
        value_ = builder_.CreateCall(proc, params);
    }
}

void LLVMCodeGen::setRefMode(bool deref) {
    deref_ctx.push(deref);
}

void LLVMCodeGen::restoreRefMode() {
    deref_ctx.pop();
}

bool LLVMCodeGen::deref() const {
    if (deref_ctx.empty()) {
        return false;
    }
    return deref_ctx.top();
}
