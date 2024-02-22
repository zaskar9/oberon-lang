/*
 * Simple tree-walk code generator to build LLVM IR for the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#include "LLVMIRBuilder.h"

#include <vector>
#include <llvm/IR/Verifier.h>
#include "system/PredefinedProcedure.h"

using std::vector;

LLVMIRBuilder::LLVMIRBuilder(CompilerConfig &config, LLVMContext &builder, Module *module) :
        NodeVisitor(), config_(config), logger_(config_.logger()), builder_(builder), module_(module),
        value_(), values_(), types_(), functions_(), strings_(), deref_ctx(), level_(0), function_(),
        attrs_(AttrBuilder(builder)) {
    attrs_
        .addAttribute(Attribute::NoInline)
        .addAttribute(Attribute::NoUnwind)
        .addAttribute(Attribute::OptimizeNone)
        .addAttribute(Attribute::StackProtect)
#ifndef _LLVM_LEGACY
        .addAttribute(Attribute::getWithUWTableKind(builder, UWTableKind::Default))
#endif
        ;
}

void LLVMIRBuilder::build(ASTContext *ast) {
    ast_ = ast;
    ast->getTranslationUnit()->accept(*this);
}

void LLVMIRBuilder::visit(ModuleNode &node) {
    module_->setModuleIdentifier(node.getIdentifier()->name());
    // allocate global variables
    for (size_t i = 0; i < node.getVariableCount(); i++) {
        auto variable = node.getVariable(i);
        auto type = getLLVMType(variable->getType());
        auto value = new GlobalVariable(*module_, type, false,
                                        (variable->getIdentifier()->isExported() ?
                                            GlobalValue::ExternalLinkage : GlobalValue::PrivateLinkage),
                                        Constant::getNullValue(type), variable->getIdentifier()->name());
        value->setAlignment(getLLVMAlign(variable->getType()));
        values_[variable] = value;
    }
    // generate external procedure signatures
    for (size_t i = 0; i < ast_->getExternalProcedureCount(); i++) {
        procedure(*ast_->getExternalProcedure(i));
    }
    // generate procedure signatures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        procedure(*node.getProcedure(i));
    }
    // generate code for procedures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
    // generate code for body
    auto body = module_->getOrInsertFunction(node.getIdentifier()->name(), builder_.getInt32Ty());
    function_ = ::cast<Function>(body.getCallee());
    auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
    builder_.SetInsertPoint(entry);
    level_ = node.getLevel() + 1;
    // generate code to initialize imports
    for (size_t i = 0; i < node.getImportCount(); ++i) {
        node.getImport(i)->accept(*this);
    }
    // initialize array sizes
    for (size_t i = 0; i < node.getVariableCount(); ++i) {
        auto var = node.getVariable(i);
        value_ = values_[var];
        arrayInitializers(var->getType());
    }
    // generate code for statements
    if (node.statements()->getStatementCount() > 0) {
        node.statements()->accept(*this);
    }
    // generate code for exit code
    if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
        builder_.CreateRet(builder_.getInt32(0));
    }
    // generate main to enable linking of executable
    if (config_.hasFlag(Flag::ENABLE_MAIN)) {
        auto main = module_->getOrInsertFunction("main", builder_.getInt32Ty());
        function_ = ::cast<Function>(main.getCallee());
        entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
        builder_.SetInsertPoint(entry);
        value_ = builder_.CreateCall(body, {});
        builder_.CreateRet(value_);
    }
    // verify the module
    verifyModule(*module_, &errs());
}

void LLVMIRBuilder::visit(ProcedureNode &node) {
    if (node.isExtern()) {
        return;
    }
    if (node.getProcedureCount() > 0) {
        logger_.error(node.pos(), "found unsupported nested procedures in " + to_string(node.getIdentifier()) + ".");
    }
    function_ = functions_[&node];
    function_->addFnAttrs(attrs_);
    // function_->addFnAttr(Attribute::AttrKind::NoInline);
    auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
    builder_.SetInsertPoint(entry);
    Function::arg_iterator args = function_->arg_begin();
    for (auto &param : node.getType()->parameters()) {
        auto arg = args++;
        arg->addAttr(Attribute::AttrKind::NoUndef);
        // arg->setName(param->getIdentifier()->name());
        bool varType = param->getType()->isArray() || param->getType()->isRecord() || param->getType()->isProcedure();
        Type *type = param->isVar() || varType ? builder_.getPtrTy() : getLLVMType(param->getType());
        Value *value = builder_.CreateAlloca(type, nullptr, param->getIdentifier()->name());
        builder_.CreateStore(arg, value);
        if (param->isVar() || varType) {
            value = builder_.CreateLoad(type, value);
        }
        values_[param.get()] = value;
    }
    for (size_t i = 0; i < node.getVariableCount(); i++) {
        auto var = node.getVariable(i);
        value_ = builder_.CreateAlloca(getLLVMType(var->getType()), nullptr, var->getIdentifier()->name());
        arrayInitializers(var->getType());
        values_[var] = value_;
    }
    level_ = node.getLevel() + 1;
    node.statements()->accept(*this);
    if (node.getType()->getReturnType() == nullptr) {
        builder_.CreateRetVoid();
    }
    auto block = builder_.GetInsertBlock();
    if (block->getTerminator() == nullptr) {
        if (node.getType()->getReturnType() != nullptr && !block->empty()) {
            logger_.error(node.pos(),
                          "function \"" + to_string(node.getIdentifier()) + "\" has no return statement.");
        } else {
            builder_.CreateUnreachable();
        }
    }
    verifyFunction(*function_, &errs());
}

void LLVMIRBuilder::arrayInitializers(TypeNode *base) {
    vector<Value *> indices;
    indices.push_back(builder_.getInt32(0));
    arrayInitializers(base, base, indices);
}

void LLVMIRBuilder::arrayInitializers(TypeNode *base, TypeNode *type, vector<Value *> &indices) {
    if (type->isArray()) {
        auto array_t = dynamic_cast<ArrayTypeNode *>(type);
        indices.push_back(builder_.getInt32(0));   // the array dimension is the first field in the struct
        Value *value = builder_.CreateInBoundsGEP(getLLVMType(base), value_, indices);
        // TODO support for multi-dimensional arrays
        builder_.CreateStore(builder_.getInt64(array_t->lengths()[0]), value);
        indices.pop_back();
        indices.push_back(builder_.getInt32(1));   // the array is the second field in the struct
        indices.push_back(builder_.getInt32(0));   // select the first element of the array
        arrayInitializers(base, array_t->getMemberType(), indices);
        indices.pop_back();
        indices.pop_back();
    } else if (type->isRecord()) {
        auto record_t = dynamic_cast<RecordTypeNode *>(type);
        for (size_t i = 0; i < record_t->getFieldCount(); ++i) {
            indices.push_back(builder_.getInt32(i));
            arrayInitializers(base, record_t->getField(i)->getType(), indices);
            indices.pop_back();
        }
    }
}


void LLVMIRBuilder::visit(ImportNode &node) {
    std::string name = node.getModule()->name();
    auto type = FunctionType::get(builder_.getInt32Ty(), {});
    auto fun = module_->getOrInsertFunction(name, type);
    if (fun) {
        value_ = builder_.CreateCall(fun, {});
    } else {
        logger_.error(node.pos(), "undefined procedure: " + name + ".");
    }
}

void LLVMIRBuilder::visit(QualifiedStatement &node) {
    auto proc = dynamic_cast<ProcedureNode *>(node.dereference());
    staticCall(proc, node.ident(), node.selectors());
}

void LLVMIRBuilder::visit(QualifiedExpression &node) {
    auto decl = node.dereference();
    auto level = decl->getLevel();
    if (level == 0 || level == 1) /* universe or global level */ {
        value_ = values_[decl];
    } else if (level == level_) /* same procedure level */ {
        value_ = values_[decl];
    } else if (level > level_) /* parent procedure level */ {
        logger_.error(node.pos(), "referencing variables of parent procedures is not yet supported.");
    } else /* error */ {
        logger_.error(node.pos(), "cannot reference variable of child procedure.");
    }
    auto type = decl->getType();
    if (type->isProcedure()) {
        staticCall(dynamic_cast<ProcedureNode *>(decl), node.ident(), node.selectors());
        return;
    }
    type = selectors(type, node.selectors().begin(), node.selectors().end());
    if (deref()) {
        value_ = builder_.CreateLoad(getLLVMType(type), value_);
    }
}

TypeNode *LLVMIRBuilder::selectors(TypeNode *base, SelectorIterator start, SelectorIterator end) {
    auto selector_t = base;
    auto value = value_;
    vector<Value *> indices;
    indices.push_back(builder_.getInt32(0));
    for (auto it = start; it != end; ++it) {
        auto sel = (*it).get();
        if (sel->getNodeType() == NodeType::parameter) {
            auto params = dynamic_cast<ActualParameters *>(sel);
            auto type = dynamic_cast<ProcedureTypeNode *>(selector_t);
            vector<Value*> values;
            parameters(type, params, values);
            auto funTy = getLLVMType(type);
            // output the GEP up to the procedure call
            value = processGEP(base, value, indices);
            // create a load to dereference the function pointer
            value = builder_.CreateLoad(funTy, value);
            value_ = builder_.CreateCall(::cast<FunctionType>(funTy), value, values);
            selector_t = type->getReturnType();
        } else if (sel->getNodeType() == NodeType::array_type) {
            // handle array index access
            indices.push_back(builder_.getInt32(1));   // the array is the second field in the struct
            setRefMode(true);
            // TODO support for multi-dimensional arrays
            dynamic_cast<ArrayIndex*>(sel)->indices()[0]->accept(*this);
            indices.push_back(value_);
            restoreRefMode();
            value = processGEP(base, value, indices);
            selector_t = dynamic_cast<ArrayTypeNode*>(selector_t)->getMemberType();
            base = selector_t;
        } else if (sel->getNodeType() == NodeType::pointer_type) {
            // output the GEP up to the pointer
            value = processGEP(base, value, indices);
            // create a load to dereference the pointer
            value = builder_.CreateLoad(getLLVMType(selector_t), value);
            selector_t = dynamic_cast<PointerTypeNode *>(selector_t)->getBase();
            base = selector_t;
        } else if (sel->getNodeType() == NodeType::record_type) {
            // handle record field access
            auto field = dynamic_cast<RecordField *>(sel)->getField();
            auto record_t = dynamic_cast<RecordTypeNode *>(selector_t);
            for (size_t pos = 0; pos < record_t->getFieldCount(); pos++) {
                if (field == record_t->getField(pos)) {
                    indices.push_back(builder_.getInt32(pos));
                    break;
                }
            }
            value = processGEP(base, value, indices);
            selector_t = field->getType();
            base = selector_t;
        } else if (sel->getNodeType() == NodeType::type) {
            logger_.error(sel->pos(), "type-guards are not yet supported.");
        } else {
            logger_.error(sel->pos(), "unsupported selector.");
        }
    }
    // clean up
    if (indices.size() > 1) {
        value_ = processGEP(base, value, indices);
    } else {
        value_ = value;
    }
    return selector_t;
}

void LLVMIRBuilder::parameters(ProcedureTypeNode *proc, ActualParameters *actuals, vector<llvm::Value *> &values) {
    for (size_t i = 0; i < actuals->parameters().size(); i++) {
        auto param = actuals->parameters()[i].get();
        auto type = param->getType();
        bool deref = i >= proc->parameters().size();
        if (type->isArray() || type->isRecord() || type->isProcedure()) {
            setRefMode(deref);
        } else {
            setRefMode(deref || !proc->parameters()[i]->isVar());
        }
        param->accept(*this);
        cast(*param);
        values.push_back(value_);
        restoreRefMode();
    }
}

TypeNode *LLVMIRBuilder::staticCall(ProcedureNode *proc, QualIdent *ident, Selectors &selectors) {
    auto type = dynamic_cast<ProcedureTypeNode *>(proc->getType());
    std::vector<Value*> values;
    ActualParameters *params = nullptr;
    if (!selectors.empty()) {
        params = dynamic_cast<ActualParameters *>(selectors[0].get());
    }
    parameters(type, params, values);
    if (proc->isPredefined()) {
        value_ = predefinedCall(dynamic_cast<PredefinedProcedure *>(proc), ident, params, values);
    } else {
        auto fun = module_->getFunction(qualifiedName(proc));
        if (fun) {
            value_ = builder_.CreateCall(fun, values);
        } else {
            logger_.error(ident->start(), "undefined procedure: " + to_string(*ident) + ".");
        }
    }
    return this->selectors(proc->getType()->getReturnType(), selectors.begin() + 1, selectors.end());
}

void LLVMIRBuilder::visit(ConstantDeclarationNode &) {}

void LLVMIRBuilder::visit(FieldNode &) {}

void LLVMIRBuilder::visit(ParameterNode &) {}

void LLVMIRBuilder::visit(VariableDeclarationNode &) {}

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
    value_ = strings_[val];
    if (!value_) {
        strings_[val] = builder_.CreateGlobalStringPtr(val, ".str");;
        value_ = strings_[val];
    }
}

void LLVMIRBuilder::visit(NilLiteralNode &node) {
    auto type = getLLVMType(node.getCast());
    value_ = ConstantPointerNull::get((PointerType*) type);
}

void LLVMIRBuilder::visit(SetLiteralNode &node) {
    value_ = ConstantInt::get(builder_.getInt32Ty(), (unsigned int) node.value().to_ulong());
}

void LLVMIRBuilder::visit(RangeLiteralNode &node) {
    value_ = ConstantInt::get(builder_.getInt32Ty(), (unsigned int) node.value().to_ulong());
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
            if (type->isSet()) {
                value_ = builder_.CreateXor(ConstantInt::get(builder_.getInt32Ty(), 0xffffffff), value_);
            } else {
                value_ = type->isReal() ? builder_.CreateFNeg(value_) : builder_.CreateNeg(value_);
            }
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
            logger_.error(node.pos(), "binary operator in unary expression.");
            break;
        default: value_ = nullptr;
            logger_.error(node.pos(), "unknown operator,");
            break;
    }
}

void LLVMIRBuilder::visit(BinaryExpressionNode &node) {
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
    } else if (node.getLeftExpression()->getType()->isSet() || node.getRightExpression()->getType()->isSet()) {
        node.getRightExpression()->accept(*this);
        cast(*node.getRightExpression());
        auto rhs = value_;
        switch (node.getOperator()) {
            case OperatorType::PLUS:
                value_ = builder_.CreateOr(lhs, rhs);
                break;
            case OperatorType::MINUS:
                value_ = builder_.CreateXor(ConstantInt::get(builder_.getInt32Ty(), 0xffffffff), rhs);
                value_ = builder_.CreateAnd(lhs, value_);
                break;
            case OperatorType::TIMES:
                value_ = builder_.CreateAnd(lhs, rhs);
                break;
            case OperatorType::DIVIDE:
                value_ = builder_.CreateXor(lhs, rhs);
                break;
            case OperatorType::LEQ:
                value_ = builder_.CreateXor(ConstantInt::get(builder_.getInt32Ty(), 0xffffffff), rhs);
                value_ = builder_.CreateAnd(lhs, value_);
                value_ = builder_.CreateIsNull(value_);
                break;
            case OperatorType::GEQ:
                value_ = builder_.CreateXor(ConstantInt::get(builder_.getInt32Ty(), 0xffffffff), lhs);
                value_ = builder_.CreateAnd(rhs, value_);
                value_ = builder_.CreateIsNull(value_);
                break;
            case OperatorType::IN:
                if (!node.getLeftExpression()->getType()->isInteger()) {
                    logger_.error(node.getLeftExpression()->pos(), "integer expression expected.");
                }
                value_ = ConstantInt::get(builder_.getInt32Ty(), 0x1);
                value_ = builder_.CreateAnd(value_, value_);
                value_ = builder_.CreateShl(value_, lhs);
                value_ = builder_.CreateAnd(value_, rhs);
                value_ = builder_.CreateIsNotNull(value_);
                break;
            default:
                value_ = nullptr;
                logger_.error(node.pos(), "operator " + to_string(node.getOperator()) + " not applicable here.");
                break;
        }
    } else {
        node.getRightExpression()->accept(*this);
        cast(*node.getRightExpression());
        bool floating = node.getLeftExpression()->getType()->isReal() || node.getRightExpression()->getType()->isReal();
        auto rhs = value_;
        switch (node.getOperator()) {
            case OperatorType::PLUS:
                value_ = floating ? builder_.CreateFAdd(lhs, rhs) : builder_.CreateAdd(lhs, rhs);
                break;
            case OperatorType::MINUS:
                value_ = floating ? builder_.CreateFSub(lhs, rhs) : builder_.CreateSub(lhs, rhs);
                break;
            case OperatorType::TIMES:
                value_ = floating ? builder_.CreateFMul(lhs, rhs) : builder_.CreateMul(lhs, rhs);
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
                value_ = floating ? builder_.CreateFCmpUEQ(lhs, rhs) : builder_.CreateICmpEQ(lhs, rhs);
                break;
            case OperatorType::NEQ:
                value_ = floating ? builder_.CreateFCmpUNE(lhs, rhs) : builder_.CreateICmpNE(lhs, rhs);
                break;
            case OperatorType::LT:
                value_ = floating ? builder_.CreateFCmpULT(lhs, rhs) :builder_.CreateICmpSLT(lhs, rhs);
                break;
            case OperatorType::GT:
                value_ = floating ? builder_.CreateFCmpUGT(lhs, rhs) : builder_.CreateICmpSGT(lhs, rhs);
                break;
            case OperatorType::LEQ:
                value_ = floating ? builder_.CreateFCmpULE(lhs, rhs) : builder_.CreateICmpSLE(lhs, rhs);
                break;
            case OperatorType::GEQ:
                value_ = floating ? builder_.CreateFCmpUGE(lhs, rhs) : builder_.CreateICmpSGE(lhs, rhs);
                break;
            case OperatorType::AND:
            case OperatorType::OR:
                // unreachable code due to the if-branch of this else-branch
                break;
            case OperatorType::NOT:
            case OperatorType::NEG:
                value_ = nullptr;
                logger_.error(node.pos(), "unary operator " + to_string(node.getOperator()) + " in binary expression.");
                break;
            default:
                value_ = nullptr;
                logger_.error(node.pos(), "operator " + to_string(node.getOperator()) + " not applicable here.");
                break;
        }
    }
}

void LLVMIRBuilder::visit(RangeExpressionNode &node) {
    Value *ones = ConstantInt::get(builder_.getInt32Ty(), 0xffffffff);
    Value *result = builder_.CreateAnd(ones, ones);
    setRefMode(true);
    node.getLower()->accept(*this);
    Value *lower = value_;
    restoreRefMode();
    result = builder_.CreateShl(result, lower);
    setRefMode(true);
    node.getUpper()->accept(*this);
    Value *upper = value_;
    restoreRefMode();
    Value *diff = builder_.CreateSub(ConstantInt::get(builder_.getInt32Ty(), 31), upper);
    Value *sum = builder_.CreateAdd(diff, lower);
    result = builder_.CreateLShr(result, sum);
    value_ = builder_.CreateShl(result, lower);
}

void LLVMIRBuilder::visit(SetExpressionNode &node) {
    Value *zero = ConstantInt::get(builder_.getInt32Ty(), 0x0);
    Value *result = builder_.CreateXor(zero, zero);
    for (auto &elem : node.elements()) {
        setRefMode(true);
        elem->accept(*this);
        if (elem->getNodeType() != NodeType::range && elem->getNodeType() != NodeType::range_expression) {
            value_ = builder_.CreateShl(ConstantInt::get(builder_.getInt32Ty(), 0x1), value_);
        }
        result = builder_.CreateOr(result, value_);
        restoreRefMode();
    }
    value_ = result;
}

void LLVMIRBuilder::visit(TypeDeclarationNode &) {}

void LLVMIRBuilder::visit(ArrayTypeNode &) {}

void LLVMIRBuilder::visit(BasicTypeNode &) {}

void LLVMIRBuilder::visit(ProcedureTypeNode &) {}

void LLVMIRBuilder::visit(RecordTypeNode &) {}

void LLVMIRBuilder::visit(PointerTypeNode &) {}

void LLVMIRBuilder::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        node.getStatement(i)->accept(*this);
        // Check whether basic block is already terminated
        if (builder_.GetInsertBlock()->getTerminator()) {
            break;
        }
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

void LLVMIRBuilder::visit(IfThenElseNode &node) {
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

void LLVMIRBuilder::visit(ElseIfNode &) {}

void LLVMIRBuilder::visit([[maybe_unused]] LoopNode &node) {
    // TODO code generation for general loop
}

void LLVMIRBuilder::visit(WhileLoopNode &node) {
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

void LLVMIRBuilder::visit(RepeatLoopNode &node) {
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

void LLVMIRBuilder::visit(ForLoopNode &node) {
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

void LLVMIRBuilder::visit(ReturnNode &node) {
    if (node.getValue()) {
        setRefMode(true);
        node.getValue()->accept(*this);
        cast(*node.getValue());
        restoreRefMode();
        value_ = builder_.CreateRet(value_);
    } else {
        value_ = builder_.CreateRetVoid();
    }
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

Value *
LLVMIRBuilder::predefinedCall(PredefinedProcedure *proc, QualIdent *ident,
                              ActualParameters *actuals, std::vector<Value *> &params) {
    auto type = dynamic_cast<ProcedureTypeNode *>(proc->getType());
    ProcKind kind = proc->getKind();
    if (kind == ProcKind::NEW) {
        auto fun = module_->getFunction("malloc");
        if (!fun) {
            auto funTy = FunctionType::get(builder_.getPtrTy(), {builder_.getInt64Ty()}, false);
            fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "malloc", module_);
            fun->addFnAttr(Attribute::getWithAllocSizeArgs(builder_.getContext(), 0, {}));
            fun->addParamAttr(0, Attribute::NoUndef);
        }
        std::vector<Value *> values;
        auto ptr = (PointerTypeNode *) actuals->parameters()[0]->getType();
        auto layout = module_->getDataLayout();
        auto base = ptr->getBase();
        values.push_back(ConstantInt::get(builder_.getInt64Ty(), layout.getTypeAllocSize(getLLVMType(base))));
        value_ = builder_.CreateCall(FunctionCallee(fun), values);
        // TODO remove next line (bit-cast) once non-opaque pointers are no longer supported
        value_ = builder_.CreateBitCast(value_, getLLVMType(ptr));
        Value *value = builder_.CreateStore(value_, params[0]);
        value_ = builder_.CreateLoad(builder_.getPtrTy(), params[0]);
        arrayInitializers(base);
        return value;
    } else if (kind == ProcKind::FREE) {
        auto fun = module_->getFunction("free");
#ifdef _LLVM_LEGACY
        auto voidTy = PointerType::get(builder_.getVoidTy(), 0);
#else
        auto voidTy = builder_.getPtrTy();
#endif
        if (!fun) {
            auto funTy = FunctionType::get(builder_.getVoidTy(), {voidTy}, false);
            fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "free", module_);
            fun->addParamAttr(0, Attribute::NoUndef);
        }
        std::vector<Value *> values;
        values.push_back(builder_.CreateLoad(voidTy, params[0]));
        builder_.CreateCall(FunctionCallee(fun), values);
#ifdef _LLVM_LEGACY
        auto ptr = (PointerTypeNode*) node.getActualParameter(0)->getType();
        void_t = (PointerType*) getLLVMType(ptr);
#endif
        return builder_.CreateStore(ConstantPointerNull::get(voidTy), params[0]);
    } else if (kind == ProcKind::INC || kind == ProcKind::DEC) {
        auto target = getLLVMType(actuals->parameters()[0]->getType());
        Value *delta;
        if (params.size() > 2) {
            auto param = actuals->parameters()[2].get();
            logger_.error(param->pos(), "more actual than formal parameters.");
            return value_;
        } else if (params.size() > 1) {
            auto source = params[1]->getType();
            auto param = actuals->parameters()[0].get();
            if (!param->getType()->isInteger()) {
                logger_.error(param->pos(), "type mismatch: expected integer type, found " +
                                            to_string(param->getType()) + ".");
                return value_;
            }
            delta = params[1];
            if (target->getIntegerBitWidth() > source->getIntegerBitWidth()) {
                delta = builder_.CreateSExt(delta, target);
            } else if (target->getIntegerBitWidth() < source->getIntegerBitWidth()) {
                logger_.warning(param->pos(), "type mismatch: truncating " + to_string(param->getType()) + " to " +
                                              to_string(type->parameters()[0]->getType()) + " may lose data.");
                delta = builder_.CreateTrunc(delta, target);
            }
        } else {
            delta = ConstantInt::get(target, 1);
        }
        Value *value = builder_.CreateLoad(target, params[0]);
        if (kind == ProcKind::INC) {
            value = builder_.CreateAdd(value, delta);
        } else {
            value = builder_.CreateSub(value, delta);
        }
        return builder_.CreateStore(value, params[0]);
    } else if (kind == ProcKind::LSL) {
        return builder_.CreateShl(params[0], params[1]);
    } else if (kind == ProcKind::ASR) {
        return builder_.CreateAShr(params[0], params[1]);
    } else if (kind == ProcKind::ROL) {
        Value *lhs = builder_.CreateShl(params[0], params[1]);
        Value *delta = builder_.CreateSub(builder_.getInt64(64), params[1]);
        Value *rhs = builder_.CreateLShr(params[0], delta);
        return builder_.CreateOr(lhs, rhs);
    } else if (kind == ProcKind::ROR) {
        Value *lhs = builder_.CreateLShr(params[0], params[1]);
        Value *delta = builder_.CreateSub(builder_.getInt64(64), params[1]);
        Value *rhs = builder_.CreateShl(params[0], delta);
        return builder_.CreateOr(lhs, rhs);
    } else if (kind == ProcKind::ODD) {
        auto paramTy = params[0]->getType();
        value_ = builder_.CreateSRem(params[0], ConstantInt::get(paramTy, 2));
        return builder_.CreateICmpEQ(value_, ConstantInt::get(paramTy, 1));
    } else if (kind == ProcKind::HALT) {
        auto fun = module_->getFunction("exit");
        if (!fun) {
            auto funTy = FunctionType::get(builder_.getVoidTy(), {builder_.getInt32Ty()}, false);
            fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "exit", module_);
            fun->addFnAttr(Attribute::NoReturn);
            fun->addParamAttr(0, Attribute::NoUndef);
        }
        builder_.CreateCall(FunctionCallee(fun), {params[0]});
        return builder_.CreateUnreachable();
    } else if (kind == ProcKind::ASSERT) {
        auto fun = module_->getFunction("abort");
        if (!fun) {
            auto funTy = FunctionType::get(builder_.getVoidTy(), {}, false);
            fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "abort", module_);
            fun->addFnAttr(Attribute::Cold);
            fun->addFnAttr(Attribute::NoReturn);
        }
        auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
        auto abort = BasicBlock::Create(builder_.getContext(), "abort", function_);
        builder_.CreateCondBr(params[0], tail, abort);
        builder_.SetInsertPoint(abort);
        builder_.CreateCall(FunctionCallee(fun), {});
        value_ = builder_.CreateUnreachable();
        builder_.SetInsertPoint(tail);
        return value_;
    } else if (kind == ProcKind::LEN) {
        auto array_t = getLLVMType(actuals->parameters()[0]->getType());
        value_ = builder_.CreateInBoundsGEP(array_t, params[0], {builder_.getInt32(0), builder_.getInt32(0)});
        value_ = builder_.CreateLoad(builder_.getInt64Ty(), value_);
        // auto array = dynamic_cast<ArrayTypeNode *>(actuals->parameters()[0]->getType());
        // value_ = builder_.getInt64(array->getDimension());
        return value_;
    } else if (kind == ProcKind::INCL) {
        Value *value = builder_.CreateShl(ConstantInt::get(builder_.getInt32Ty(), 0x1), params[1]);
        value_ = builder_.CreateLoad(builder_.getInt32Ty(), params[0]);
        value_ = builder_.CreateOr(value_, value);
        return builder_.CreateStore(value_, params[0]);
    } else if (kind == ProcKind::EXCL) {
        Value *value = builder_.CreateShl(ConstantInt::get(builder_.getInt32Ty(), 0x1), params[1]);
        value = builder_.CreateXor(ConstantInt::get(builder_.getInt32Ty(), 0xffffffff), value);
        value_ = builder_.CreateLoad(builder_.getInt32Ty(), params[0]);
        value_ = builder_.CreateAnd(value_, value);
        return builder_.CreateStore(value_, params[0]);
    } else if (kind == ProcKind::ORD) {
        auto param = actuals->parameters()[0].get();
        if (param->getType()->isBoolean()) {
            Value *value = builder_.CreateAlloca(builder_.getInt32Ty());
            auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
            auto return1 = BasicBlock::Create(builder_.getContext(), "true", function_);
            auto return0 = BasicBlock::Create(builder_.getContext(), "false", function_);
            builder_.CreateCondBr(params[0], return1, return0);
            builder_.SetInsertPoint(return1);
            builder_.CreateStore(ConstantInt::get(builder_.getInt32Ty(), 0x1), value);
            builder_.CreateBr(tail);
            builder_.SetInsertPoint(return0);
            builder_.CreateStore(ConstantInt::get(builder_.getInt32Ty(), 0x0), value);
            builder_.CreateBr(tail);
            builder_.SetInsertPoint(tail);
            value_ = builder_.CreateLoad(builder_.getInt32Ty(), value);
        } else if (param->getType()->isChar()) {
            logger_.error(param->pos(), "type CHAR is not yet supported.");
        } else if (param->getType()->isSet()) {
            value_ = params[0];
        } else {
            logger_.error(param->pos(), "type mismatch: BOOLEAN, CHAR, or SET expected.");
        }
        return value_;
    } else {
        logger_.error(ident->start(), "unsupported predefined procedure: " + to_string(*ident) + ".");
        // to generate correct LLVM IR, the current value is returned (no-op).
        return value_;
    }
}

void LLVMIRBuilder::procedure(ProcedureNode &node) {
    auto name = qualifiedName(&node);
    if (module_->getFunction(name)) {
        logger_.error(node.pos(), "Function " + name + " already defined.");
        return;
    }
    std::vector<Type*> params;
    for (auto &param : node.getType()->parameters()) {
        auto param_t = getLLVMType(param->getType());
        bool varType = param->getType()->isArray() || param->getType()->isRecord() || param->getType()->isProcedure();
        params.push_back(param->isVar() || varType ? param_t->getPointerTo() : param_t);
    }
    auto type = FunctionType::get(getLLVMType(node.getType()->getReturnType()), params, node.getType()->hasVarArgs());
    auto callee = module_->getOrInsertFunction(name, type);
    functions_[&node] = ::cast<Function>(callee.getCallee());
}

std::string LLVMIRBuilder::qualifiedName(DeclarationNode *node) const {
    if (node->getNodeType() == NodeType::procedure) {
        auto proc = dynamic_cast<ProcedureNode *>(node);
        if (proc->isExtern() && node->getModule() == ast_->getTranslationUnit()) {
            return node->getIdentifier()->name();
        }
    }
    return node->getModule()->getIdentifier()->name() + "_" + node->getIdentifier()->name();
}

Value *LLVMIRBuilder::processGEP(TypeNode *base, Value *value, vector<Value *> &indices) {
    if (indices.size() > 1) {
        auto result = builder_.CreateInBoundsGEP(getLLVMType(base), value, indices);
        indices.clear();
        indices.push_back(builder_.getInt32(0));
        return result;
    }
    return value;
}

Type* LLVMIRBuilder::getLLVMType(TypeNode *type) {
    Type* result = nullptr;
    if (type == nullptr) {
        result = builder_.getVoidTy();
    } else if (types_[type] != nullptr) {
        result = types_[type];
    } else if (type->getNodeType() == NodeType::array_type) {
        auto array_t = dynamic_cast<ArrayTypeNode *>(type);
        // create a struct that stores the size and the elements of the array
        // TODO support for multi-dimensional arrays
        result = ArrayType::get(getLLVMType(array_t->getMemberType()), array_t->lengths()[0]);
        result = StructType::create(builder_.getContext(), {builder_.getInt64Ty(), result});
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
            struct_t->setName("record." + record_t->getIdentifier()->name());
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
            result = builder_.getPtrTy();
        } else if (type->kind() == TypeKind::SET) {
            result = builder_.getInt32Ty();
        }
        types_[type] = result;
    }
    if (result == nullptr) {
        logger_.error(type->pos(), "cannot map type to LLVM intermediate representation.");
        exit(1);
    }
    return result;
}

MaybeAlign LLVMIRBuilder::getLLVMAlign(TypeNode *type) {
    auto layout = module_->getDataLayout();
    if (type->getNodeType() == NodeType::array_type) {
        auto array_t = dynamic_cast<ArrayTypeNode*>(type);
        auto int_align = layout.getPrefTypeAlign(builder_.getInt64Ty());
        auto mem_align = getLLVMAlign(array_t->getMemberType());
        if (int_align.value() > mem_align->value()) {
            return int_align;
        }
        return mem_align;
    } else if (type->getNodeType() == NodeType::record_type) {
        auto record_t = dynamic_cast<RecordTypeNode *>(type);
        uint64_t size = 0;
        for (size_t i = 0; i < record_t->getFieldCount(); i++) {
            auto field_t = record_t->getField(i)->getType();
            size = std::max(size, getLLVMAlign(field_t)->value());
        }
        return MaybeAlign(size);
    } else if (type->getNodeType() == NodeType::pointer_type) {
        return { layout.getPointerPrefAlignment() };
    } else if (type->getNodeType() == NodeType::basic_type) {
        return { layout.getPrefTypeAlign(getLLVMType(type)) };
    }
    return {};
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
