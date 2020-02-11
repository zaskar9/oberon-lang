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
        value->setAlignment(variable->getType()->getSize());
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
    } else /* error */ {
        std::cerr << "Cannot reference value of child procedure." << std::endl;
    }
    auto type = ref->getType();
    if (node.getSelectorCount() > 0) {
        auto base = value_;
        std::vector<Value*> indices;
        indices.push_back(builder_.getInt32(0));
        for (size_t i = 0; i < node.getSelectorCount(); i++) {
            if (type->getNodeType() == NodeType::array_type) {
                // handle array access
                bool deref = deref_;
                deref_ = true;
                node.getSelector(i)->accept(*this);
                indices.push_back(value_);
                deref_ = deref;
            } else if (type->getNodeType() == NodeType::record_type) {
                // todo handle record access
            }
        }
        value_ = builder_.CreateInBoundsGEP(getLLVMType(type), base, indices);
    }

    if (deref_) {
        if (ref->getNodeType() == NodeType::variable) {
            value_ = builder_.CreateLoad(value_);
        } else if (ref->getNodeType() == NodeType::parameter) {
            auto param = dynamic_cast<ParameterNode*>(ref);
            if (param->isVar()) {
                value_ = builder_.CreateLoad(value_);
            }
        }
    }
}

void LLVMCodeGen::visit(ConstantDeclarationNode &node) { }

void LLVMCodeGen::visit(FieldNode &node) { }

void LLVMCodeGen::visit(ParameterNode &node) { }

void LLVMCodeGen::visit(VariableDeclarationNode &node) { }

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
    Value* lhs = value_;
    node.getRightExpression()->accept(*this);
    Value* rhs = value_;
    switch (node.getOperator()) {
        case OperatorType::PLUS: value_ = builder_.CreateAdd(lhs, rhs); break;
        case OperatorType::MINUS: value_= builder_.CreateSub(lhs, rhs); break;
        case OperatorType::TIMES: value_ = builder_.CreateMul(lhs, rhs); break;
        case OperatorType::DIV: value_ = builder_.CreateSDiv(lhs, rhs); break;
        case OperatorType::MOD: value_ = builder_.CreateSRem(lhs, rhs); break;
        case OperatorType::AND: value_ = builder_.CreateAdd(lhs, rhs); break;
        case OperatorType::OR: value_ = builder_.CreateOr(lhs, rhs); break;
        case OperatorType::EQ: value_ = builder_.CreateICmpEQ(lhs, rhs); break;
        case OperatorType::NEQ: value_ = builder_.CreateICmpNE(lhs, rhs); break;
        case OperatorType::LT: value_ = builder_.CreateICmpSLT(lhs, rhs); break;
        case OperatorType::GT: value_ = builder_.CreateICmpSGT(lhs, rhs); break;
        case OperatorType::LEQ: value_ = builder_.CreateICmpSLE(lhs, rhs); break;
        case OperatorType::GEQ: value_ = builder_.CreateICmpSGE(lhs, rhs); break;
    }
}

void LLVMCodeGen::visit(TypeDeclarationNode &node) { }

void LLVMCodeGen::visit(ArrayTypeNode &node) { }

void LLVMCodeGen::visit(BasicTypeNode &node) { }

void LLVMCodeGen::visit(RecordTypeNode &node) { }

void LLVMCodeGen::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        node.getStatement(i)->accept(*this);
    }
}

void LLVMCodeGen::visit(AssignmentNode &node) {
    deref_ = true;
    node.getRvalue()->accept(*this);
    Value* rValue = value_;
    deref_ = false;
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
    node.getCondition()->accept(*this);
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
        elsif->getCondition()->accept(*this);
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

void LLVMCodeGen::visit(LoopNode& node) { }

void LLVMCodeGen::visit(WhileLoopNode& node) { }

void LLVMCodeGen::visit(RepeatLoopNode& node) { }

void LLVMCodeGen::visit(ForLoopNode& node) {
    // initialize loop counter
    deref_ = true;
    node.getLow()->accept(*this);
    auto start = value_;
    deref_ = false;
    node.getCounter()->accept(*this);
    auto counter = value_;
    value_ = builder_.CreateStore(start, counter);
    // check whether to skip loop body
    deref_ = true;
    node.getHigh()->accept(*this);
    auto end = value_;
    node.getCounter()->accept(*this);
    counter = value_;
    deref_ = false;
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
    deref_ = true;
    node.getCounter()->accept(*this);
    deref_ = false;
    counter = builder_.CreateAdd(value_, builder_.getInt32(node.getStep()));
    node.getCounter()->accept(*this);
    auto lValue = value_;
    builder_.CreateStore(counter, lValue);
    // check whether to exit loop body
    deref_ = true;
    node.getHigh()->accept(*this);
    end = value_;
    node.getCounter()->accept(*this);
    counter = value_;
    deref_ = false;
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
    deref_ = true;
    node.getValue()->accept(*this);
    deref_ = false;
    value_ = builder_.CreateRet(value_);
}

Type* LLVMCodeGen::getLLVMType(TypeNode *type, bool isPtr) {
    Type* result = nullptr;
    if (type == nullptr) {
        result = builder_.getVoidTy();
    } else if (type->getNodeType() == NodeType::array_type) {
        auto array_t = dynamic_cast<ArrayTypeNode*>(type);
        result = ArrayType::get(getLLVMType(array_t->getMemberType()), array_t->getDimension());
    } else {
        switch (type->getNodeType()) {
            case NodeType::array_type:
                break;
            case NodeType::basic_type:
                if (type == BasicTypeNode::BOOLEAN) {
                    result = builder_.getInt1Ty();
                } else if (type == BasicTypeNode::INTEGER) {
                    result = builder_.getInt32Ty();
                } else if (type == BasicTypeNode::STRING) {
                    result = builder_.getInt8PtrTy();
                }
                break;
            case NodeType::record_type:
                break;
        }
    }
    if (result != nullptr) {
        if (isPtr) {
            result = PointerType::get(result, 0);
        }
    } else {
        std::cerr << "Cannot map type " << *type << " to LLVM." << std::endl;
    }
    return result;
}

void LLVMCodeGen::call(CallNode &node) {
    std::vector<Value*> params;
    std::string name = node.getProcedure()->getName();
    size_t fp_cnt = node.getProcedure()->getParameterCount();
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        deref_ = (i < fp_cnt) ? !node.getProcedure()->getParameter(i)->isVar() : true;
        node.getParameter(i)->accept(*this);
        params.push_back(value_);
        deref_ = false;
    }
    auto proc = module_->getFunction(name);
    if (proc) {
        value_ = builder_.CreateCall(proc, params);
    }
}
