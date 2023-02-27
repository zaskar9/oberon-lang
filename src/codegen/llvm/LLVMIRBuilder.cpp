/*
 * Simple tree-walk code generator to build LLVM IR for the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#include "LLVMIRBuilder.h"
#include "system/PredefinedProcedure.h"
#include <llvm/IR/Verifier.h>

LLVMIRBuilder::LLVMIRBuilder(Logger *logger, LLVMContext &context, Module *module) : NodeVisitor(),
        logger_(logger), builder_(context), module_(module), value_(), values_(), types_(), functions_(),
        deref_ctx(), level_(0), function_() { }

void LLVMIRBuilder::build(Node *node) {
    node->accept(*this);
}

void LLVMIRBuilder::visit(ModuleNode &node) {
    module_->setModuleIdentifier(node.getIdentifier()->name());
    // allocate global variables
    for (size_t i = 0; i < node.getVariableCount(); i++) {
        auto variable = node.getVariable(i);
        auto type = getLLVMType(variable->getType());
        auto value = new GlobalVariable(*module_, type, false,
                                        (variable->getIdentifier()->isExported() ? GlobalValue::ExternalLinkage
                                                                            : GlobalValue::PrivateLinkage),
                                        Constant::getNullValue(type), variable->getIdentifier()->name());
        value->setAlignment(getLLVMAlign(variable->getType()));
        values_[variable] = value;
    }
    // generate external procedure signatures
    for (size_t i = 0; i < node.getExternalProcedureCount(); i++) {
        proc(*node.getExternalProcedure(i));
    }
    // generate procedure signatures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        proc(*node.getProcedure(i));
    }
    // generate code for procedures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
    if (node.getStatements()->getStatementCount() > 0) {
        // generate code for main
        auto main = module_->getOrInsertFunction("main", builder_.getInt32Ty());
        function_ = ::cast<Function>(main.getCallee());
        auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
        builder_.SetInsertPoint(entry);
        level_ = node.getLevel() + 1;
        node.getStatements()->accept(*this);
        builder_.CreateRet(builder_.getInt32(0));
    }
    // verify the module
    llvm::verifyModule(*module_, &errs());
}

void LLVMIRBuilder::visit(ProcedureNode &node) {
    if (node.getProcedureCount() > 0) {
        logger_->error(node.pos(), "found unsupported nested procedures in " + to_string(node.getIdentifier()) + ".");
    }
    if (!node.isExtern()) {
        function_ = functions_[&node];
        // function_->addFnAttr(Attribute::AttrKind::NoInline);
        Function::arg_iterator args = function_->arg_begin();
        for (size_t i = 0; i < node.getFormalParameterCount(); i++) {
            auto arg = args++;
            auto param = node.getFormalParameter(i);
            arg->setName(param->getIdentifier()->name());
            values_[param] = arg;
            // function_->addParamAttr(i, Attribute::AttrKind::NoUndef);
        }
        auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
        builder_.SetInsertPoint(entry);
        for (size_t i = 0; i < node.getVariableCount(); i++) {
            auto var = node.getVariable(i);
            auto value = builder_.CreateAlloca(getLLVMType(var->getType()), nullptr, var->getIdentifier()->name());
            values_[var] = value;
        }
        level_ = node.getLevel() + 1;
        node.getStatements()->accept(*this);
        if (node.getReturnType() == nullptr) {
            builder_.CreateRetVoid();
        }
        auto block = builder_.GetInsertBlock();
        if (block->getTerminator() == nullptr) {
            if (node.getReturnType() != nullptr && !block->empty()) {
                logger_->error(node.pos(),
                               "function \"" + to_string(node.getIdentifier()) + "\" has no return statement.");
            } else {
                builder_.CreateUnreachable();
            }
        }
        llvm::verifyFunction(*function_, &errs());
    }
}

void LLVMIRBuilder::visit([[maybe_unused]] ImportNode &node) { }

void LLVMIRBuilder::visit(ValueReferenceNode &node) {
    if (node.getNodeType() == NodeType::procedure_call) {
        call(node);
        return;
    }
    auto ref = node.dereference();
    auto level = ref->getLevel();
    if (level == 1) /* global level */ {
        value_ = values_[ref];
    } else if (level == level_) /* same procedure level */ {
        value_ = values_[ref];
    } else if (level > level_) /* parent procedure level */ {
        logger_->error(ref->pos(), "referencing variables of parent procedures is not yet supported.");
    } else /* error */ {
        logger_->error(ref->pos(), "cannot reference variable of child procedure.");
    }
    auto type = ref->getType();
    if (type->getNodeType() == NodeType::array_type ||
        type->getNodeType() == NodeType::record_type ||
        type->getNodeType() == NodeType::pointer_type) {
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
        std::vector<Value *> indices;
        indices.push_back(builder_.getInt32(0));
        for (size_t i = 0; i < node.getSelectorCount(); i++) {
            auto sel = node.getSelector(i);
            if (selector_t->getNodeType() == NodeType::array_type) {
                // handle array index access
                setRefMode(true);
                dynamic_cast<ArrayIndex*>(sel)->getExpression()->accept(*this);
                indices.push_back(value_);
                restoreRefMode();
                selector_t = dynamic_cast<ArrayTypeNode*>(selector_t)->getMemberType();
            } else if (selector_t->getNodeType() == NodeType::record_type) {
                // handle record field access
                auto field = dynamic_cast<RecordField *>(sel)->getField();
                auto record_t = dynamic_cast<RecordTypeNode*>(selector_t);
                for (size_t pos = 0; pos < record_t->getFieldCount(); pos++) {
                    if (field == record_t->getField(pos)) {
                        indices.push_back(builder_.getInt32(pos));
                        break;
                    }
                }
                selector_t = field->getType();
            } else if (selector_t->getNodeType() == NodeType::pointer_type) {
                // output the GEP up to the pointer
                if (indices.size() > 1) {
                    base = builder_.CreateInBoundsGEP(getLLVMType(type), base, indices);
                    indices.clear();
                    indices.push_back(builder_.getInt32(0));
                }
                // create a load to dereference the pointer
                base = builder_.CreateLoad(getLLVMType(selector_t), base);
                selector_t = dynamic_cast<PointerTypeNode *>(selector_t)->getBase();
                type = selector_t;
            } else {
                logger_->error(sel->pos(), "unexpected selector.");
            }
        }
        // clean up
        if (indices.size() > 1) {
            value_ = builder_.CreateInBoundsGEP(getLLVMType(type), base, indices);
        } else {
            value_ = base;
        }
        type = selector_t;
    }
    if (deref()) {
        if (ref->getNodeType() == NodeType::variable) {
            value_ = builder_.CreateLoad(getLLVMType(type), value_);
        } else if (ref->getNodeType() == NodeType::parameter) {
            auto param = dynamic_cast<ParameterNode*>(ref);
            // load value of parameter that is either passed by reference or is an array or
            // record parameter since getelementptr only computes the address
            if (param->isVar() || param->getType()->getNodeType() == NodeType::array_type ||
                                  param->getType()->getNodeType() == NodeType::record_type ||
                                  param->getType()->getNodeType() == NodeType::pointer_type) {
                value_ = builder_.CreateLoad(getLLVMType(type), value_);
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
    value_ = node.value() ? builder_.getTrue() : builder_.getFalse();
}

void LLVMIRBuilder::visit(IntegerLiteralNode &node) {
    if (node.isLong()) {
        value_ = ConstantInt::getSigned(builder_.getInt64Ty(), node.value());
    } else {
        value_ = ConstantInt::getSigned(builder_.getInt32Ty(), (int) node.value());
    }
    cast(node);
}

void LLVMIRBuilder::visit(RealLiteralNode &node) {
    if (node.isLong()) {
        value_ = ConstantFP::get(builder_.getDoubleTy(), node.value());
    } else {
        value_ = ConstantFP::get(builder_.getFloatTy(), (float) node.value());
    }
    cast(node);
}

void LLVMIRBuilder::visit(StringLiteralNode &node) {
    std::string val = node.value();
    value_ = builder_.CreateGlobalStringPtr(val, ".str");
}

void LLVMIRBuilder::visit(NilLiteralNode &node) {
    auto type = getLLVMType(node.getCast());
    value_ = ConstantPointerNull::get((PointerType*) type);
}

void LLVMIRBuilder::visit(UnaryExpressionNode &node) {
    auto type = node.getType();
    node.getExpression()->accept(*this);
    cast(*node.getExpression());
    switch (node.getOperator()) {
        case OperatorType::NOT:
            value_ = builder_.CreateNot(value_);
            break;
        case OperatorType::NEG:
            value_ = type->isReal() ? builder_.CreateFNeg(value_) : builder_.CreateNeg(value_);
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
    auto type = node.getType();
    node.getLeftExpression()->accept(*this);
    cast(*node.getLeftExpression());
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
        cast(*node.getRightExpression());
        auto rhs = value_;
        switch (node.getOperator()) {
            case OperatorType::PLUS:
                value_ = type->isReal() ? builder_.CreateFAdd(lhs, rhs) : builder_.CreateAdd(lhs, rhs);
                break;
            case OperatorType::MINUS:
                value_ = type->isReal() ? builder_.CreateFSub(lhs, rhs) : builder_.CreateSub(lhs, rhs);
                break;
            case OperatorType::TIMES:
                value_ = type->isReal() ? builder_.CreateFMul(lhs, rhs) : builder_.CreateMul(lhs, rhs);
                break;
            case OperatorType::DIVIDE:
                value_ = builder_.CreateFDiv(lhs, rhs);
                break;
            case OperatorType::DIV:
                value_ = builder_.CreateSDiv(lhs, rhs);
                break;
            case OperatorType::MOD:
                value_ = builder_.CreateSRem(lhs, rhs);
                break;
            case OperatorType::EQ:
                value_ = type->isReal() ? builder_.CreateFCmpUEQ(lhs, rhs) : builder_.CreateICmpEQ(lhs, rhs);
                break;
            case OperatorType::NEQ:
                value_ = type->isReal() ? builder_.CreateFCmpUNE(lhs, rhs) : builder_.CreateICmpNE(lhs, rhs);
                break;
            case OperatorType::LT:
                value_ = type->isReal() ? builder_.CreateFCmpULT(lhs, rhs) :builder_.CreateICmpSLT(lhs, rhs);
                break;
            case OperatorType::GT:
                value_ = type->isReal() ? builder_.CreateFCmpUGT(lhs, rhs) : builder_.CreateICmpSGT(lhs, rhs);
                break;
            case OperatorType::LEQ:
                value_ = type->isReal() ? builder_.CreateFCmpULE(lhs, rhs) : builder_.CreateICmpSLE(lhs, rhs);
                break;
            case OperatorType::GEQ:
                value_ = type->isReal() ? builder_.CreateFCmpUGE(lhs, rhs) : builder_.CreateICmpSGE(lhs, rhs);
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
                logger_->error(node.pos(), "unknown operator.");
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

void LLVMIRBuilder::visit([[maybe_unused]] ProcedureTypeNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] RecordTypeNode &node) {
    // Node does not need code generation.
}

void LLVMIRBuilder::visit([[maybe_unused]] PointerTypeNode &node) {
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
    cast(*node.getRvalue());
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
    auto step = dynamic_cast<IntegerLiteralNode*>(node.getStep())->value();
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

void LLVMIRBuilder::cast(ExpressionNode &node) {
    auto target = node.getCast();
    auto source = node.getType();
    if (target && target != source) {
        if (source->isInteger()) {
            if (target->isInteger()) {
                if (target->getSize() > source->getSize()) {
                    value_ = builder_.CreateSExt(value_, getLLVMType(target));
                } else {
                    value_ = builder_.CreateTrunc(value_, getLLVMType(target));
                }
            } else if (target->isReal()) {
                value_ = builder_.CreateSIToFP(value_, getLLVMType(target));
            }
        } else if (source->isReal()) {
            if (target->isReal()) {
                if (target->getSize() > source->getSize()) {
                    value_ = builder_.CreateFPExt(value_, getLLVMType(target));
                } else {
                    value_ = builder_.CreateFPTrunc(value_, getLLVMType(target));
                }
            }
        }
    }
}

Value *LLVMIRBuilder::callPredefined(ProcedureNodeReference &node, std::string name, std::vector<Value *> &params) {
    if (name == New::NAME) {
        auto type = FunctionType::get(builder_.getInt8PtrTy(), { builder_.getInt64Ty() }, false);
        auto callee = module_->getOrInsertFunction("malloc", type);
        std::vector<Value *> values;
        auto ptr = (PointerTypeNode *) node.getActualParameter(0)->getType();
        auto layout = module_->getDataLayout();
        auto base = ptr->getBase();
        values.push_back(ConstantInt::get(builder_.getInt64Ty(), layout.getTypeAllocSize(getLLVMType(base))));
        value_ = builder_.CreateCall(callee, values);
        value_ = builder_.CreateBitCast(value_, getLLVMType(ptr)); // TODO remove once non-opaque pointers are no longer supported
        return builder_.CreateStore(value_, params[0]);
    } else if (name == Free::NAME) {
        auto type = FunctionType::get(builder_.getVoidTy(), { builder_.getPtrTy() }, false);
        auto callee = module_->getOrInsertFunction("free", type);
        std::vector<Value *> values;
        values.push_back(builder_.CreateLoad(builder_.getPtrTy(), params[0]));
        builder_.CreateCall(callee, values);
        return builder_.CreateStore(ConstantPointerNull::get(builder_.getPtrTy()), params[0]);
    } else if (name == Inc::NAME || name == Dec::NAME) {
        auto target = getLLVMType(node.getActualParameter(0)->getType());
        Value *delta;
        if (params.size() > 2) {
            auto param = node.getActualParameter(2);
            logger_->error(param->pos(), "more actual than formal parameters.");
            return value_;
        } else if (params.size() > 1) {
            auto source = params[1]->getType();
            delta = params[1];
            if (target->getIntegerBitWidth() > source->getIntegerBitWidth()) {
                delta = builder_.CreateSExt(delta, target);
            } else if (target->getIntegerBitWidth() < source->getIntegerBitWidth()) {
                delta = builder_.CreateTrunc(delta, target);
            }
        } else {
            if (target->isIntegerTy(64)) {
                delta = builder_.getInt64(1);
            } else {
                delta = builder_.getInt32(1);
            }
        }
        Value *value = builder_.CreateLoad(target, params[0]);
        if (name == Inc::NAME) {
            value = builder_.CreateAdd(value, delta);
        } else {
            value = builder_.CreateSub(value, delta);
        }
        return builder_.CreateStore(value, params[0]);
    } else if (name == Lsl::NAME || name == Asr::NAME) {
        if (name == Lsl::NAME) {
            return builder_.CreateShl(params[0], params[1]);
        }
        return builder_.CreateAShr(params[0], params[1]);
    } else if (name == Ror::NAME) {
        Value *lhs = builder_.CreateLShr(params[0], params[1]);
        Value *delta = builder_.CreateSub(builder_.getInt64(64), params[1]);
        Value *rhs = builder_.CreateShl(params[0], delta);
        return builder_.CreateOr(lhs, rhs);
    } else if (name == Rol::NAME) {
        Value *lhs = builder_.CreateShl(params[0], params[1]);
        Value *delta = builder_.CreateSub(builder_.getInt64(64), params[1]);
        Value *rhs = builder_.CreateLShr(params[0], delta);
        return builder_.CreateOr(lhs, rhs);
    }
    logger_->error(node.pos(), "unsupported predefined procedure: " + name + ".");
    // to generate correct LLVM IR, the current value is returned (no-op).
    return value_;
}

void LLVMIRBuilder::call(ProcedureNodeReference &node) {
    auto proc = dynamic_cast<ProcedureNode *>(node.dereference());
    auto ident = proc->getIdentifier();
    size_t fp_cnt = proc->getFormalParameterCount();
    for (size_t i = 0; i < node.getActualParameterCount(); i++) {
        setRefMode(i >= proc->getFormalParameterCount() || !proc->getFormalParameter(i)->isVar());
        node.getActualParameter(i)->accept(*this);
        params.push_back(value_);
        restoreRefMode();
    }
    if (proc->isPredefined()) {
        value_ = callPredefined(node, ident->name(), params);
    } else {
        auto fun = module_->getFunction(qualifiedName(ident, proc->isExtern()));
        if (fun) {
            value_ = builder_.CreateCall(fun, params);
        } else {
            logger_->error(node.pos(), "undefined procedure: " + to_string(*ident) + ".");
        }
    }
}

void LLVMIRBuilder::proc(ProcedureNode &node) {
    std::vector<Type*> params;
    for (size_t j = 0; j < node.getFormalParameterCount(); j++) {
        auto param = node.getFormalParameter(j);
        auto param_t = getLLVMType(param->getType());
        params.push_back(param->isVar() ? param_t->getPointerTo() : param_t);
    }
    auto type = FunctionType::get(getLLVMType(node.getReturnType()), params, node.hasVarArgs());
    auto name = qualifiedName(node.getIdentifier(), node.isExtern());
    auto callee = module_->getOrInsertFunction(name, type);
    functions_[&node] = ::cast<Function>(callee.getCallee());
}

std::string LLVMIRBuilder::qualifiedName(Ident *ident, bool external) const {
    if (ident->isQualified()) {
        return dynamic_cast<QualIdent *>(ident)->qualifier() + "_" + ident->name();
    }
    if (external) {
        return ident->name();
    }
    return module_->getModuleIdentifier() + "_" + ident->name();
}

Type* LLVMIRBuilder::getLLVMType(TypeNode *type) {
    Type* result = nullptr;
    if (type == nullptr) {
        result = builder_.getVoidTy();
    } else if (types_[type] != nullptr) {
        result = types_[type];
    } else if (type->getNodeType() == NodeType::array_type) {
        auto array_t = dynamic_cast<ArrayTypeNode *>(type);
        result = ArrayType::get(getLLVMType(array_t->getMemberType()), array_t->getDimension());
        types_[type] = result;
    } else if (type->getNodeType() == NodeType::record_type) {
        // create an empty struct and add it to the lookup table immediately to support recursive records
        auto struct_t = StructType::create(builder_.getContext());
        types_[type] = struct_t;
        std::vector<Type *> elem_ts;
        auto record_t = dynamic_cast<RecordTypeNode *>(type);
        for (size_t i = 0; i < record_t->getFieldCount(); i++) {
            elem_ts.push_back(getLLVMType(record_t->getField(i)->getType()));
        }
        struct_t->setBody(elem_ts);
        if (!record_t->isAnonymous()) {
            struct_t->setName("T_" + record_t->getIdentifier()->name());
        }
        result = struct_t;
    } else if (type->getNodeType() == NodeType::pointer_type) {
        auto pointer_t = dynamic_cast<PointerTypeNode *>(type);
        auto base = getLLVMType(pointer_t->getBase());
        result = PointerType::get(base, 0);
        types_[type] = result;
    } else if (type->getNodeType() == NodeType::basic_type) {
        if (type->kind() == TypeKind::BOOLEAN) {
            result = builder_.getInt1Ty();
        } else if (type->kind() == TypeKind::INTEGER) {
            result = builder_.getInt32Ty();
        } else if (type->kind() == TypeKind::LONGINT) {
            result = builder_.getInt64Ty();
        } else if (type->kind() == TypeKind::REAL) {
            result = builder_.getFloatTy();
        } else if (type->kind() == TypeKind::LONGREAL) {
            result = builder_.getDoubleTy();
        } else if (type->kind() == TypeKind::STRING) {
            result = builder_.getInt8PtrTy();
        }
        types_[type] = result;
    }
    if (result == nullptr) {
        logger_->error(type->pos(), "cannot map type to LLVM intermediate representation.");
        exit(1);
    }
    return result;
}

MaybeAlign LLVMIRBuilder::getLLVMAlign(TypeNode *type) {
    auto layout = module_->getDataLayout();
    if (type->getNodeType() == NodeType::array_type) {
        auto align = layout.getStackAlignment();
        if (type->getSize() >= align.value()) {
            return MaybeAlign(align);
        }
        auto array_t = dynamic_cast<ArrayTypeNode*>(type);
        return getLLVMAlign(array_t->getMemberType());
    } else if (type->getNodeType() == NodeType::record_type) {
        auto record_t = dynamic_cast<RecordTypeNode *>(type);
        uint64_t size = 0;
        for (size_t i = 0; i < record_t->getFieldCount(); i++) {
            auto field_t = record_t->getField(i)->getType();
            if (field_t->getNodeType() == NodeType::array_type) {
                auto array_t = dynamic_cast<ArrayTypeNode *>(field_t);
                field_t = array_t->getMemberType();
            }
            size = std::max(size, getLLVMAlign(field_t)->value());
        }
        return MaybeAlign(size);
    } else if (type->getNodeType() == NodeType::pointer_type) {
        return MaybeAlign(layout.getPointerPrefAlignment());
    } else if (type->getNodeType() == NodeType::basic_type) {
        return MaybeAlign(layout.getPrefTypeAlignment(getLLVMType(type)));
    }
    return MaybeAlign();
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
