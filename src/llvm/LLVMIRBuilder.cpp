/*
 * Simple tree-walk code generator to build LLVM IR for the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#include "LLVMIRBuilder.h"
#include <llvm/IR/Verifier.h>

LLVMIRBuilder::LLVMIRBuilder(Logger *logger, LLVMContext &context, Module *module) : NodeVisitor(),
        logger_(logger), builder_(context), module_(module), value_(), values_(), types_(), functions_(),
        deref_ctx(), level_(0), function_() { }

void LLVMIRBuilder::build(Node *node) {
    node->accept(*this);
}

void LLVMIRBuilder::visit(ModuleNode &node) {
    // allocate global variables
    for (size_t i = 0; i < node.getVariableCount(); i++) {
        auto variable = node.getVariable(i);
        auto type = getLLVMType(variable->getType());
        auto value = new GlobalVariable(*module_, type, false,
                GlobalValue::CommonLinkage, Constant::getNullValue(type), variable->getName());
        value->setAlignment(getLLVMAlign(variable->getType()));
        values_[variable] = value;
    }
    // generate procedure signatures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        std::vector<Type*> params;
        auto proc = node.getProcedure(i);
        for (size_t j = 0; j < proc->getParameterCount(); j++) {
            auto param = proc->getParameter(j);
            params.push_back(getLLVMType(param->getType(), param->isVar()));
        }
        auto type = FunctionType::get(getLLVMType(proc->getReturnType()), params, proc->hasVarArgs());
        auto callee = module_->getOrInsertFunction(proc->getName(), type);
        functions_[proc] = cast<Function>(callee.getCallee());
    }
    // generate code for procedures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
    // generate code for main
    auto main = module_->getOrInsertFunction("main", getLLVMType(BasicTypeNode::INTEGER));
    function_ = cast<Function>(main.getCallee());
    auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
    builder_.SetInsertPoint(entry);
    level_ = node.getLevel() + 1;
    node.getStatements()->accept(*this);
    builder_.CreateRet(builder_.getInt32(0));
    // verify the module
    llvm::verifyModule(*module_, &errs());
}

void LLVMIRBuilder::visit(ProcedureNode &node) {
    if (node.getProcedureCount() > 0) {
        logger_->error(node.pos(), "Found unsupported nested procedures in " + node.getName() + ".");
    }
    if (!node.isExtern()) {
        function_ = functions_[&node];
        Function::arg_iterator args = function_->arg_begin();
        for (size_t i = 0; i < node.getParameterCount(); i++) {
            auto arg = args++;
            auto param = node.getParameter(i);
            arg->setName(param->getName());
            values_[param] = arg;
        }
        auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
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
                logger_->error(node.pos(), "function \"" + node.getName() + "\" has no return statement.");
            } else {
                builder_.CreateUnreachable();
            }
        }
        llvm::verifyFunction(*function_, &errs());
    }
}

void LLVMIRBuilder::visit(ValueReferenceNode &node) {
    auto ref = node.dereference();
    auto level = ref->getLevel();
    if (level == 1) /* global level */ {
        value_ = values_[ref];
    } else if (level == level_) /* same procedure level */ {
        value_ = values_[ref];
    } else if (level > level_) /* parent procedure level */ {
        logger_->error(ref->pos(), "Referencing variables of parent procedures is not yet supported.");
    } else /* error */ {
        logger_->error(ref->pos(), "Cannot reference variable of child procedure.");
    }
    auto type = ref->getType();
    if (type->getNodeType() == NodeType::array_type ||
        type->getNodeType() == NodeType::record_type) {
        auto selector_t = type;
        auto base = value_;
        if (ref->getNodeType() == NodeType::parameter) {
            auto param = dynamic_cast<ParameterNode*>(ref);
            // create a base address within the stack frame of the current procedure for
            // array and record parameters that are passed by value
            if (!param->isVar()) {
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
                auto decl = dynamic_cast<ValueReferenceNode*>(selector)->dereference();
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

void LLVMIRBuilder::visit([[maybe_unused]] TypeReferenceNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] ConstantDeclarationNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] FieldNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] ParameterNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] VariableDeclarationNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit(BooleanLiteralNode &node) {
    value_ = node.getValue() ? builder_.getTrue() : builder_.getFalse();
}

void LLVMIRBuilder::visit(IntegerLiteralNode &node) {
    value_ = ConstantInt::getSigned(builder_.getInt32Ty(), node.getValue());
}

void LLVMIRBuilder::visit(StringLiteralNode &node) {
    std::string val = node.getValue();
    value_ = builder_.CreateGlobalStringPtr(val, ".str");
}

void LLVMIRBuilder::visit(FunctionCallNode &node) {
    call(node);
}

void LLVMIRBuilder::visit(UnaryExpressionNode &node) {
    node.getExpression()->accept(*this);
    switch (node.getOperator()) {
        case OperatorType::NOT:
            value_ = builder_.CreateNot(value_);
            break;
        case OperatorType::NEG:
            value_ = builder_.CreateNeg(value_);
            break;
        case OperatorType::AND:
        case OperatorType::OR:
        case OperatorType::PLUS:
        case OperatorType::MINUS:
        case OperatorType::TIMES:
        case OperatorType::DIV:
        case OperatorType::MOD:
        case OperatorType::EQ:
        case OperatorType::NEQ:
        case OperatorType::LT:
        case OperatorType::GT:
        case OperatorType::LEQ:
        case OperatorType::GEQ:
            value_ = nullptr;
            logger_->error(node.pos(), "binary operator in unary expression.");
            break;
        default: value_ = nullptr;
            logger_->error(node.pos(), "unknown operator,");
            break;
    }
}

void LLVMIRBuilder::visit(BinaryExpressionNode &node) {
    node.getLeftExpression()->accept(*this);
    auto lhs = value_;
    // Logical operators AND and OR are treated explicitly in order to enable short-circuiting.
    if (node.getOperator() == OperatorType::AND || node.getOperator() == OperatorType::OR) {
        // Create one block to evaluate the right-hand side and one to skip it.
        auto eval = BasicBlock::Create(builder_.getContext(), "eval", function_);
        auto skip = BasicBlock::Create(builder_.getContext(), "skip", function_);
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
            case OperatorType::AND:
            case OperatorType::OR:
                // unreachable code due to the if-branch of this else-branch
                break;
            case OperatorType::NOT:
            case OperatorType::NEG:
                value_ = nullptr;
                logger_->error(node.pos(), "unary operator in binary expression.");
                break;
            default:
                value_ = nullptr;
                logger_->error(node.pos(), " unknown operator.");
                break;
        }
    }
}

void LLVMIRBuilder::visit([[maybe_unused]] TypeDeclarationNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] ArrayTypeNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] BasicTypeNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] RecordTypeNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        node.getStatement(i)->accept(*this);
    }
}

void LLVMIRBuilder::visit(AssignmentNode &node) {
    setRefMode(true);
    node.getRvalue()->accept(*this);
    Value* rValue = value_;
    restoreRefMode();
    node.getLvalue()->accept(*this);
    Value* lValue = value_;
    value_ = builder_.CreateStore(rValue, lValue);
}

void LLVMIRBuilder::visit(IfThenElseNode& node) {
    auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
    auto if_true = BasicBlock::Create(builder_.getContext(), "if_true", function_);
    auto if_false = tail;
    if (node.hasElse() || node.hasElseIf()) {
        if_false = BasicBlock::Create(builder_.getContext(), "if_false", function_);
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
        auto elsif_true = BasicBlock::Create(builder_.getContext(), "elsif_true", function_);
        auto elsif_false = tail;
        if (node.hasElse() || i + 1 < node.getElseIfCount()) {
            elsif_false = BasicBlock::Create(builder_.getContext(), "elsif_false", function_);
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

void LLVMIRBuilder::visit([[maybe_unused]] ElseIfNode& node) {
    // Code for this node is generated in the context of if-then-else node.
}

void LLVMIRBuilder::visit(ProcedureCallNode& node) {
    call(node);
}

void LLVMIRBuilder::visit([[maybe_unused]] LoopNode& node) {
    // todo code generation for general loop
}

void LLVMIRBuilder::visit(WhileLoopNode& node) {
    auto body = BasicBlock::Create(builder_.getContext(), "loop_body", function_);
    auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
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

void LLVMIRBuilder::visit(RepeatLoopNode& node) {
    auto body = BasicBlock::Create(builder_.getContext(), "loop_body", function_);
    auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
    builder_.CreateBr(body);
    builder_.SetInsertPoint(body);
    node.getStatements()->accept(*this);
    setRefMode(true);
    node.getCondition()->accept(*this);
    restoreRefMode();
    builder_.CreateCondBr(value_, tail, body);
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(ForLoopNode& node) {
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
    auto body = BasicBlock::Create(builder_.getContext(), "loop_body", function_);
    auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
    auto step = dynamic_cast<IntegerLiteralNode*>(node.getStep())->getValue();
    if (step > 0) {
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
    counter = builder_.CreateAdd(value_, ConstantInt::getSigned(builder_.getInt32Ty(), step));
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
    if (step > 0) {
        value_ = builder_.CreateICmpSGT(counter, end);
    } else {
        value_ = builder_.CreateICmpSLT(counter, end);
    }
    builder_.CreateCondBr(value_, tail, body);
    // after loop
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(ReturnNode& node) {
    setRefMode(true);
    node.getValue()->accept(*this);
    restoreRefMode();
    value_ = builder_.CreateRet(value_);
}

Type* LLVMIRBuilder::getLLVMType(TypeNode *type, bool isPtr) {
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
        auto struct_t = StructType::create(elem_ts);
        struct_t->setName("record." + record_t->getName());
        result = struct_t;
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
        logger_->error(type->pos(), "Cannot map type to LLVM intermediate representation.");
    }
    return result;
}

MaybeAlign LLVMIRBuilder::getLLVMAlign(TypeNode *type, bool isPtr) {
    auto layout = module_->getDataLayout();
    if (type->getNodeType() == NodeType::array_type) {
        auto align = layout.getStackAlignment();
        if (type->getSize() >= align.value()) {
            return MaybeAlign(align);
        }
        auto array_t = dynamic_cast<ArrayTypeNode*>(type);
        return getLLVMAlign(array_t->getMemberType());
    } else if (type->getNodeType() == NodeType::record_type) {
        auto record_t = dynamic_cast<RecordTypeNode*>(type);
        uint64_t size = 0;
        for (size_t i = 0; i < record_t->getFieldCount(); i++) {
            auto field_t = record_t->getField(i)->getType();
            if (field_t->getNodeType() == NodeType::array_type) {
                auto array_t = dynamic_cast<ArrayTypeNode*>(field_t);
                field_t = array_t->getMemberType();
            }
            size = std::max(size, getLLVMAlign(field_t)->value());
        }
        return MaybeAlign(size);
    } else if (type->getNodeType() == NodeType::basic_type) {
        return MaybeAlign(layout.getPrefTypeAlignment(getLLVMType(type, isPtr)));
    }
    return MaybeAlign();

}

void LLVMIRBuilder::call(ProcedureNodeReference &node) {
    std::vector<Value*> params;
    std::string name = node.dereference()->getName();
    size_t fp_cnt = node.dereference()->getParameterCount();
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        setRefMode(i < fp_cnt ? !node.dereference()->getParameter(i)->isVar() : true);
        node.getParameter(i)->accept(*this);
        params.push_back(value_);
        restoreRefMode();
    }
    auto proc = module_->getFunction(name);
    if (proc) {
        value_ = builder_.CreateCall(proc, params);
    } else {
        logger_->error(node.pos(), "Undefined procedure: " + name + ".");
    }
}

void LLVMIRBuilder::setRefMode(bool deref) {
    deref_ctx.push(deref);
}

void LLVMIRBuilder::restoreRefMode() {
    deref_ctx.pop();
}

bool LLVMIRBuilder::deref() const {
    if (deref_ctx.empty()) {
        return false;
    }
    return deref_ctx.top();
}
