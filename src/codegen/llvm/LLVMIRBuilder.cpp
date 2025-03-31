/*
 * Simple tree-walk code generator to build LLVM IR for the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#include "LLVMIRBuilder.h"

#include <limits>
#include <csignal>
#include <sstream>
#include <vector>
#include <llvm/IR/Verifier.h>
#include "system/PredefinedProcedure.h"

using std::ostringstream;
using std::vector;

LLVMIRBuilder::LLVMIRBuilder(CompilerConfig &config, LLVMContext &builder, Module *module) :
        NodeVisitor(), config_(config), logger_(config_.logger()), builder_(builder), module_(module),
		triple_(module->getTargetTriple()),
        value_(), values_(),
        types_(), typeDopes_(), valueDopes_(), ptrTypes_(), recTypeIds_(), recTypeTds_(), valueTds_(),
        functions_(), strings_(), deref_ctx(), scope_(0), scopes_(), function_(), attrs_(AttrBuilder(builder)) {
    attrs_
            .addAttribute(Attribute::NoInline)
            .addAttribute(Attribute::NoUnwind)
            .addAttribute(Attribute::OptimizeNone)
#ifndef _LLVM_LEGACY
            .addAttribute(Attribute::getWithUWTableKind(builder, UWTableKind::Default))
#endif
            ;
#ifndef __MINGW32__
    if (!config_.hasFlag(Flag::NO_STACK_PROTECT)) {
        attrs_.addAttribute(Attribute::StackProtect);
    }
#endif
    recordTdTy_ = StructType::create(builder_.getContext(), {builder_.getPtrTy(), builder_.getInt32Ty()});
    recordTdTy_->setName("record.record_td");
}

void LLVMIRBuilder::build(ASTContext *ast) {
    ast_ = ast;
    ast->getTranslationUnit()->accept(*this);
}

void LLVMIRBuilder::visit(ModuleNode &node) {
    module_->setModuleIdentifier(node.getIdentifier()->name());
    // Generate type declarations and cache corresponding LLVM types
    for (size_t i = 0; i < node.getTypeDeclarationCount(); ++i) {
        node.getTypeDeclaration(i)->accept(*this);
    }
    // Allocate global variables
    string prefix = node.getIdentifier()->name() + "_";
    for (size_t i = 0; i < node.getVariableCount(); ++i) {
        auto variable = node.getVariable(i);
        auto type = getLLVMType(variable->getType());
        bool expo = variable->getIdentifier()->isExported();
        auto value = new GlobalVariable(*module_, type, false,
                                        expo ? GlobalValue::ExternalLinkage : GlobalValue::InternalLinkage,
                                        Constant::getNullValue(type),
                                        expo ? prefix + variable->getIdentifier()->name() : variable->getIdentifier()->name());
        value->setAlignment(module_->getDataLayout().getPreferredAlign(value));
        values_[variable] = value;
    }
    // Generate external procedure signatures
    for (size_t i = 0; i < ast_->getExternalProcedureCount(); ++i) {
        procedure(*ast_->getExternalProcedure(i));
    }
    // Generate procedure signatures
    for (size_t i = 0; i < node.getProcedureCount(); ++i) {
        procedure(*node.getProcedure(i));
    }
    // Generate code for procedures
    for (size_t i = 0; i < node.getProcedureCount(); ++i) {
        node.getProcedure(i)->accept(*this);
    }
    // Generate code for body
    auto body = module_->getOrInsertFunction(node.getIdentifier()->name(), builder_.getInt32Ty());
    function_ = dyn_cast<Function>(body.getCallee());
    if (triple_.isOSWindows() && !config_.hasFlag(Flag::ENABLE_MAIN)) {
        function_->setDLLStorageClass(llvm::GlobalValue::DLLStorageClassTypes::DLLExportStorageClass);
    }
    auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
    builder_.SetInsertPoint(entry);
    scope_ = node.getScope() + 1;
    // Generate code to initialize imports
    for (auto &import : node.imports()) {
        import->accept(*this);
    }
    // Initialize array sizes
    for (size_t i = 0; i < node.getVariableCount(); ++i) {
        auto var = node.getVariable(i);
        value_ = values_[var];
    }
    // Generate code for statements
    if (node.statements()->getStatementCount() > 0) {
        node.statements()->accept(*this);
    }
    // Generate code for exit code
    if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
        builder_.CreateRet(builder_.getInt32(0));
    }
    // Generate main to enable linking of executable
    if (config_.hasFlag(Flag::ENABLE_MAIN)) {
        auto main = module_->getOrInsertFunction("main", builder_.getInt32Ty());
        function_ = ::cast<Function>(main.getCallee());
        entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
        builder_.SetInsertPoint(entry);
        value_ = builder_.CreateCall(body, {});
        builder_.CreateRet(value_);
    }
    // Verify the module
    verifyModule(*module_, &errs());
}

void LLVMIRBuilder::visit(ProcedureNode &node) {
    if (node.isExtern() || node.isImported()) {
        return;
    }
    string name = node.getIdentifier()->name();
    scopes_.push_back(name);
    if (node.getProcedureCount() > 0) {
        logger_.error(node.pos(), "found unsupported nested procedures in " + name + ".");
    }
    // Generate type declarations and cache corresponding LLVM types
    for (size_t i = 0; i < node.getTypeDeclarationCount(); ++i) {
        node.getTypeDeclaration(i)->accept(*this);
    }
    function_ = functions_[&node];
    function_->addFnAttrs(attrs_);
    // function_->addFnAttr(Attribute::AttrKind::NoInline);
    auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
    builder_.SetInsertPoint(entry);
    Function::arg_iterator args = function_->arg_begin();
    // Allocate space for parameters
    for (auto &param : node.getType()->parameters()) {
        auto arg = args++;
        arg->addAttr(Attribute::AttrKind::NoUndef);
        // Variable and structured parameters, i.e., arrays and records, are passed by reference
        Type *type = param->isVar() || param->getType()->isStructured() ? builder_.getPtrTy() : getLLVMType(param->getType());
        Value *value = builder_.CreateAlloca(type, nullptr, param->getIdentifier()->name());
        builder_.CreateStore(arg, value);
        values_[param.get()] = value;
        if (param->getType()->isArray() && dynamic_cast<ArrayTypeNode*>(param->getType())->isOpen()) {
            // Handle the parameter containing the dope vector of an open array
            arg = args++;
            value = builder_.CreateAlloca(builder_.getPtrTy(), nullptr, param->getIdentifier()->name() + ".dv");
            builder_.CreateStore(arg, value);
            valueDopes_[param.get()] = value;
        } else if (param->getType()->isRecord() && param->isVar()) {
            // Handle the parameter containing the type descriptor of a variable record
            arg = args++;
            value = builder_.CreateAlloca(builder_.getPtrTy(), nullptr, param->getIdentifier()->name() + ".td");
            builder_.CreateStore(arg, value);
            valueTds_[param.get()] = value;
        }
    }
    // Allocate space for variables
    for (size_t i = 0; i < node.getVariableCount(); ++i) {
        auto variable = node.getVariable(i);
        value_ = builder_.CreateAlloca(getLLVMType(variable->getType()), nullptr, variable->getIdentifier()->name());
        values_[variable] = value_;
        if (variable->getType()->isArray()) {
            valueDopes_[variable] = typeDopes_[dynamic_cast<ArrayTypeNode*>(variable->getType())];
        }
    }
    scope_ = node.getScope() + 1;
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
    scopes_.pop_back();
}

void LLVMIRBuilder::visit(ImportNode &node) {
    std::string name = node.getModule()->name();
    if (name == "SYSTEM") {
        return; // No initialization for pseudo modules
    }
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
    createStaticCall(proc, node.ident(), node.selectors());
}

void LLVMIRBuilder::visit(QualifiedExpression &node) {
    auto decl = node.dereference();
    if (decl->getNodeType() == NodeType::type) {
        // If the qualified expression refers to a type, no code has to be generated.
        return;
    }
    auto scope = decl->getScope();
    if (scope == 0 || scope == 1) /* universe or global scope */ {
        value_ = values_[decl];
    } else if (scope == scope_) /* same procedure scope */ {
        value_ = values_[decl];
    } else if (scope > scope_) /* parent procedure scope */ {
        logger_.error(node.pos(), "referencing variables of parent procedures is not yet supported.");
    } else /* error */ {
        logger_.error(node.pos(), "cannot reference variable of child procedure.");
    }
    auto type = decl->getType();
    if (type->isProcedure()) {
        createStaticCall(dynamic_cast<ProcedureNode *>(decl), node.ident(), node.selectors());
        return;
    }
    if (!value_ && decl->getNodeType() == NodeType::variable) {
        // Looks like we came across a reference to an imported variable.
        auto variable = dynamic_cast<VariableDeclarationNode *>(decl);
        auto value = new GlobalVariable(*module_, getLLVMType(variable->getType()), false,
                                        GlobalValue::ExternalLinkage, nullptr, qualifiedName(decl));
        value->setAlignment(module_->getDataLayout().getPreferredAlign(value));
        values_[decl] = value;
        value_ = value;
    }
    if (decl->getNodeType() == NodeType::parameter) {
        auto param = dynamic_cast<ParameterNode *>(decl);
        // Since variable and structured parameters are passed as pointers
        // for performance reasons, they need to be explicitly de-referenced.
        if (param->isVar() || param->getType()->isStructured()) {
            value_ = builder_.CreateLoad(builder_.getPtrTy(), value_);
        }
    }
    type = selectors(&node, type, node.selectors().begin(), node.selectors().end());
    if (deref()) {
        value_ = builder_.CreateLoad(type->isStructured() ? builder_.getPtrTy() : getLLVMType(type), value_);
    }
}

Value *LLVMIRBuilder::getArrayLength(ExpressionNode *expr, uint32_t dim) {
    auto type = dynamic_cast<ArrayTypeNode *>(expr->getType());
    if (type->isOpen()) {
        Value *dopeV = getDopeVector(expr);
        return getOpenArrayLength(dopeV, type, dim);
    }
    return builder_.getInt64(type->lengths()[dim]);
}

Value *LLVMIRBuilder::getOpenArrayLength(llvm::Value *dopeV, ArrayTypeNode *type, uint32_t dim) {
    auto dopeTy = ArrayType::get(builder_.getInt64Ty(), type->dimensions());
    // dereference the pointer to the dope vector
    dopeV = builder_.CreateLoad(builder_.getPtrTy(), dopeV);
    Value *value = builder_.CreateInBoundsGEP(dopeTy, dopeV, {builder_.getInt32(0), builder_.getInt32(dim) });
    return builder_.CreateLoad(builder_.getInt64Ty(), value);
}

Value *LLVMIRBuilder::getDopeVector(ExpressionNode *expr) {
    Value *dopeV = nullptr;
    if (auto decl = dynamic_cast<QualifiedExpression *>(expr)->dereference()) {
        if (auto type = dynamic_cast<ArrayTypeNode *>(expr->getType())) {
            // Find the base array type of the parameter expression
            auto base = type->getBase();
            if (type->isOpen()) {
                dopeV = valueDopes_[decl];
            } else {
                dopeV = typeDopes_[base];
            }
            // Check whether the actual array has less dimensions than the base array due to applied array indices
            if (type->dimensions() < base->dimensions()) {
                auto delta = base->dimensions() - type->dimensions();
                auto dopeTy = ArrayType::get(builder_.getInt64Ty(), base->dimensions());
                dopeV = deref() ? builder_.CreateLoad(builder_.getPtrTy(), dopeV) : dopeV;
                dopeV = builder_.CreateInBoundsGEP(dopeTy, dopeV, {builder_.getInt32(0), builder_.getInt32(delta)});
            }
        } else if (decl->getType()->isArray()) {
            dopeV = valueDopes_[decl];
        }
    }
    if (!dopeV) {
        logger_.error(expr->pos(), "cannot determine array telemetry of expression.");
    }
    return dopeV;
}

Value *LLVMIRBuilder::getTypeDescriptor(Value *value, QualifiedExpression *expr, TypeNode *type) {
    Value *typeD = nullptr;
    auto decl = expr->dereference();
    if (type->isPointer()) {
        auto pointer_t = dynamic_cast<PointerTypeNode *>(type);
        typeD = builder_.CreateInBoundsGEP(ptrTypes_[pointer_t], value, {builder_.getInt32(0), builder_.getInt32(0)});
    } else if (decl->getType()->isRecord() && type->isRecord() && type->extends(decl->getType())) {
        typeD = valueTds_[decl];
    }
    if (!typeD) {
        logger_.error(expr->pos(), "cannot determine type description of expression.");
    }
    return typeD;
}

TypeNode *LLVMIRBuilder::selectors(QualifiedExpression *expr, TypeNode *base, SelectorIterator start, SelectorIterator end) {
    if (!base || base->isVirtual()) {
        return nullptr;
    }
    auto selector_t = base;
    auto baseTy = getLLVMType(selector_t);
    auto value = value_;
    vector<Value *> indices;
    indices.push_back(builder_.getInt32(0));
    for (auto it = start; it != end; ++it) {
        auto sel = (*it).get();
        if (sel->getNodeType() == NodeType::parameter) {
            auto params = dynamic_cast<ActualParameters *>(sel);
            auto procedure_t = dynamic_cast<ProcedureTypeNode *>(selector_t);
            vector<Value*> values;
            parameters(procedure_t, params, values);
            auto funTy = getLLVMType(procedure_t);
            // Output the GEP up to the procedure call
            value = processGEP(baseTy, value, indices);
            // Create a load to dereference the function pointer
            value = builder_.CreateLoad(funTy, value);
            value_ = builder_.CreateCall(dyn_cast<FunctionType>(funTy), value, values);
            selector_t = procedure_t->getReturnType();
            baseTy = getLLVMType(selector_t);
        } else if (sel->getNodeType() == NodeType::array_type) {
            auto array = dynamic_cast<ArrayIndex *>(sel);
            auto array_t = dynamic_cast<ArrayTypeNode *>(selector_t);
            setRefMode(true);
            Value *dopeV = nullptr;
            for (size_t i = 0; i < array->indices().size(); ++i) {
                auto index = array->indices()[i].get();
                index->accept(*this);
                if (config_.isSanitized(Trap::OUT_OF_BOUNDS) && (array_t->isOpen() || !index->isLiteral())) {
                    Value *lower = builder_.getInt64(0);
                    Value *upper;
                    if (array_t->isOpen()) {
                        dopeV = dopeV ? dopeV : getDopeVector(expr);
                        upper = getOpenArrayLength(dopeV, array_t, static_cast<uint32_t>(i));
                    } else {
                        upper = builder_.getInt64(array_t->lengths()[i]);
                    }
                    trapOutOfBounds(builder_.CreateSExt(value_, builder_.getInt64Ty()), lower, upper);
                }
                indices.push_back(value_);
            }
            restoreRefMode();
            value = processGEP(baseTy, value, indices);
            selector_t = array_t->types()[array->indices().size() - 1];
            baseTy = getLLVMType(selector_t);
        } else if (sel->getNodeType() == NodeType::pointer_type) {
            // Output the GEP up to the pointer
            value = processGEP(baseTy, value, indices);
            // Create a load to dereference the pointer
            value = builder_.CreateLoad(baseTy, value);
            // Trap NIL pointer access
            if (config_.isSanitized(Trap::NIL_POINTER)) {
                trapNILPtr(value);
            }
            auto pointer_t = dynamic_cast<PointerTypeNode *>(selector_t);
            selector_t = pointer_t->getBase();
            // Handle values of record base type
            if (selector_t->isRecord()) {
                // Skip the first field (type descriptor tag) of a leaf record type
                indices.push_back(builder_.getInt32(1));
                baseTy = ptrTypes_[pointer_t];
            } else {
                baseTy = getLLVMType(selector_t);  // TODO switch to LLVM types
            }
        } else if (sel->getNodeType() == NodeType::record_type) {
            // Handle record field access
            auto field = dynamic_cast<RecordField *>(sel)->getField();
            auto record_t = dynamic_cast<RecordTypeNode *>(selector_t);
            // Navigate through the base records
            unsigned current = field->getRecordType()->getLevel();
            for (unsigned level = current; level < record_t->getLevel(); level++) {
                indices.push_back(builder_.getInt32(0));
            }
            // Access the field by its index (increase index at levels > 0)
            indices.push_back(builder_.getInt32(current == 0 ? field->getIndex() : field->getIndex() + 1));
            // Output GEP up to the record field
            value = processGEP(baseTy, value, indices);
            selector_t = field->getType();
            baseTy = getLLVMType(selector_t);
        } else if (sel->getNodeType() == NodeType::type) {
            auto guard = dynamic_cast<Typeguard *>(sel);
            if (config_.isSanitized(Trap::TYPE_GUARD)) {
                auto guard_t = guard->getType();
                auto tmp = value;
                if (guard_t->isPointer()) {
                    value = processGEP(baseTy, value, indices);
                    tmp = builder_.CreateLoad(builder_.getPtrTy(), value);
                }
                auto cond = createTypeTest(tmp, expr, selector_t, guard_t);
                trapTypeGuard(cond);
            }
            selector_t = guard->getType();
            baseTy = getLLVMType(selector_t);
        } else {
            logger_.error(sel->pos(), "unsupported selector.");
        }
    }
    // Clean up: emit any remaining indices
    if (indices.size() > 1) {
        value_ = processGEP(baseTy, value, indices);
    } else {
        value_ = value;
    }
    return selector_t;
}

void
LLVMIRBuilder::parameters(ProcedureTypeNode *proc, ActualParameters *actuals, vector<llvm::Value *> &values, bool) {
    for (size_t i = 0; i < actuals->parameters().size(); i++) {
        auto actualParam = actuals->parameters()[i].get();
        auto actualType = actualParam->getType();
        ParameterNode *formalParam = nullptr;
        if (i < proc->parameters().size()) {
            // Non-variadic argument
            formalParam = proc->parameters()[i].get();
            if (formalParam->isVar()              // VAR parameter
                || actualType->isStructured()     // ARRAY or RECORD
                || actualType->isString()) {      // STRING literal parameter
                setRefMode(false);
            } else {
                setRefMode(true);
            }
        } else {
            // Variadic argument
            setRefMode(actualType->isBasic() || actualType->isString());
        }
        actualParam->accept(*this);
        cast(*actualParam);
        values.push_back(value_);
        restoreRefMode();
        if (formalParam) {
            auto formalType = formalParam->getType();
            if (formalType->isArray() && dynamic_cast<ArrayTypeNode*>(formalType)->isOpen()) {
                // Add a pointer to the dope vector of an open array
                if (actualType->isString() && actualParam->isLiteral()) {
                    // Create a "dope vector" on-the-fly for string literals
                    auto str = dynamic_cast<StringLiteralNode *>(actualParam);
                    Value *dopeV = builder_.CreateAlloca(ArrayType::get(builder_.getInt64Ty(), 1));
                    builder_.CreateStore(builder_.getInt64(str->value().size() + 1), dopeV);
                    values.push_back(dopeV);
                } else {
                    // Lookup the "dope vector" of explicitly defined array types
                    Value *dopeV = getDopeVector(actualParam);
                    values.push_back(dopeV);
                }
            } else if (formalType->isRecord() && formalParam->isVar()) {
                // Add a pointer to the type descriptor of the variable record
                values.push_back(recTypeTds_[dynamic_cast<RecordTypeNode *>(actualType)]);
            }
        }
    }
}

TypeNode *LLVMIRBuilder::createStaticCall(ProcedureNode *proc, QualIdent *ident, Selectors &selectors) {
    auto type = dynamic_cast<ProcedureTypeNode *>(proc->getType());
    std::vector<Value*> values;
    auto params = dynamic_cast<ActualParameters *>(selectors[0].get());
    parameters(type, params, values, proc->isExtern());
    if (proc->isPredefined()) {
        value_ = createPredefinedCall(dynamic_cast<PredefinedProcedure *>(proc), ident, params->parameters(), values);
    } else {
        auto fun = module_->getFunction(qualifiedName(proc));
        if (fun) {
            value_ = builder_.CreateCall(fun, values);
        } else {
            logger_.error(ident->start(), "undefined procedure: " + to_string(*ident) + ".");
        }
    }
    return this->selectors(nullptr, proc->getType()->getReturnType(), selectors.begin() + 1, selectors.end());
}

void LLVMIRBuilder::visit(ConstantDeclarationNode &) {}

void LLVMIRBuilder::visit(FieldNode &) {}

void LLVMIRBuilder::visit(ParameterNode &) {}

void LLVMIRBuilder::visit(VariableDeclarationNode &) {}

void LLVMIRBuilder::visit(BooleanLiteralNode &node) {
    value_ = node.value() ? builder_.getTrue() : builder_.getFalse();
}

void LLVMIRBuilder::visit(IntegerLiteralNode &node) {
    if (node.getType()->kind() == TypeKind::LONGINT) {
        value_ = ConstantInt::getSigned(builder_.getInt64Ty(), node.value());
    } else if (node.getType()->kind() == TypeKind::INTEGER) {
        value_ = ConstantInt::getSigned(builder_.getInt32Ty(), static_cast<int32_t>(node.value()));
    } else {
        value_ = ConstantInt::getSigned(builder_.getInt16Ty(), static_cast<int16_t>(node.value()));
    }
    cast(node);
}

void LLVMIRBuilder::visit(RealLiteralNode &node) {
    if (node.getType()->kind() == TypeKind::LONGREAL) {
        value_ = ConstantFP::get(builder_.getDoubleTy(), node.value());
    } else {
        value_ = ConstantFP::get(builder_.getFloatTy(), static_cast<float>(node.value()));
    }
    cast(node);
}

void LLVMIRBuilder::visit(StringLiteralNode &node) {
    string val = node.value();
    auto len = val.size() + 1;
    auto type = ArrayType::get(builder_.getInt8Ty(), len);
    value_ = strings_[val];
    if (!value_) {
        auto init = ConstantDataArray::getRaw(val, len, builder_.getInt8Ty());
        auto str = new GlobalVariable(*module_, type, true, GlobalValue::PrivateLinkage, init, ".str");
        str->setAlignment(module_->getDataLayout().getPrefTypeAlign(type));
        strings_[val] = str;
        value_ = strings_[val];
    }
    if (deref()) {
        value_ = builder_.CreateLoad(type, value_);
    }
}

void LLVMIRBuilder::visit(CharLiteralNode &node) {
    value_ = builder_.getInt8(node.value());
    cast(node);
}

void LLVMIRBuilder::visit(NilLiteralNode &node) {
    value_ = ConstantPointerNull::get((PointerType*) getLLVMType(node.getCast()));
}

void LLVMIRBuilder::visit(SetLiteralNode &node) {
    value_ = ConstantInt::get(builder_.getInt32Ty(), static_cast<uint32_t>(node.value().to_ulong()));
}

void LLVMIRBuilder::visit(RangeLiteralNode &node) {
    value_ = ConstantInt::get(builder_.getInt32Ty(), static_cast<uint32_t>(node.value().to_ulong()));
}

void LLVMIRBuilder::installTrap(Value *cond, uint8_t code) {
    auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
    auto trap = BasicBlock::Create(builder_.getContext(), "trap", function_);
    builder_.CreateCondBr(cond, tail, trap);
    builder_.SetInsertPoint(trap);
#ifndef _LLVM_20
    Function* fun = Intrinsic::getDeclaration(module_, Intrinsic::ubsantrap);
#else
    Function* fun = Intrinsic::getOrInsertDeclaration(module_, Intrinsic::ubsantrap);
#endif
    builder_.CreateCall(fun, {builder_.getInt8(code)});
    builder_.CreateUnreachable();
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::trapOutOfBounds(Value *value, Value *lower, Value *upper) {
    Value *lowerLT = builder_.CreateICmpSGE(value, lower);
    Value *upperGE = builder_.CreateICmpSLT(value, upper);
    Value *cond = builder_.CreateAnd(lowerLT, upperGE);
    installTrap(cond, static_cast<uint8_t>(Trap::OUT_OF_BOUNDS));
}

void LLVMIRBuilder::trapTypeGuard(Value *cond) {
    // auto value = builder_.CreateNot(cond);
    installTrap(cond, static_cast<uint8_t>(Trap::TYPE_GUARD));
}

void LLVMIRBuilder::trapCopyOverflow(Value *lsize, Value *rsize) {
    auto cond = builder_.CreateICmpUGE(lsize, rsize);
    installTrap(cond, static_cast<uint8_t>(Trap::COPY_OVERFLOW));
}

void LLVMIRBuilder::trapNILPtr(Value *value) {
    Value *cond = builder_.CreateIsNotNull(value);
    installTrap(cond, static_cast<uint8_t>(Trap::NIL_POINTER));
}

void LLVMIRBuilder::trapIntDivByZero(Value *divisor) {
    auto type = dyn_cast<IntegerType>(divisor->getType());
    auto cond = builder_.CreateICmpSGT(divisor, ConstantInt::get(type, 0));
    installTrap(cond, static_cast<uint8_t>(Trap::INT_DIVISION));
}

void LLVMIRBuilder::trapAssert(llvm::Value *cond) {
    installTrap(cond, static_cast<uint8_t>(Trap::ASSERT));
}

Value *LLVMIRBuilder::trapIntOverflow(Intrinsic::IndependentIntrinsics intrinsic, Value *lhs, Value *rhs) {
    auto type = dyn_cast<IntegerType>(rhs->getType());
#ifndef _LLVM_20
    Function* fun = Intrinsic::getDeclaration(module_, intrinsic, { type });
#else
    Function* fun = Intrinsic::getOrInsertDeclaration(module_, intrinsic, { type });
#endif
    auto call = builder_.CreateCall(fun, {lhs, rhs});
    auto result = builder_.CreateExtractValue(call, {0});
    auto status = builder_.CreateExtractValue(call, {1});
    auto cond = builder_.CreateXor(status, builder_.getTrue());
    installTrap(cond, static_cast<uint8_t>(Trap::INT_OVERFLOW));
    return result;
}

void LLVMIRBuilder::trapFltDivByZero(Value *divisor) {
    auto cond = builder_.CreateFCmpUNE(divisor, ConstantFP::get(divisor->getType(), 0));
    installTrap(cond, static_cast<uint8_t>(Trap::FLT_DIVISION));
}

Value *LLVMIRBuilder::createTypeTest(Value *td, TypeNode *type) {
    RecordTypeNode *record_t = dynamic_cast<RecordTypeNode *>(type);
    if (!record_t) {
        auto pointer_t = dynamic_cast<PointerTypeNode *>(type);
        record_t = dynamic_cast<RecordTypeNode *>(pointer_t->getBase());
    }
    Value *level = builder_.getInt32(record_t->getLevel());
    Value *value = builder_.CreateLoad(builder_.getPtrTy(), td);
    auto len = builder_.CreateInBoundsGEP(recordTdTy_, value, {builder_.getInt32(0), builder_.getInt32(1)});
    len = builder_.CreateLoad(builder_.getInt32Ty(), len);
    auto cond = builder_.CreateICmpULE(level, len);
    auto cur = builder_.GetInsertBlock();
    auto test = BasicBlock::Create(builder_.getContext(), "test", function_);
    auto skip = BasicBlock::Create(builder_.getContext(), "skip", function_);
    builder_.CreateCondBr(cond, test, skip);
    builder_.SetInsertPoint(test);
    auto tds = builder_.CreateInBoundsGEP(recordTdTy_, value, {builder_.getInt32(0), builder_.getInt32(0)});
    value = builder_.CreateLoad(builder_.getPtrTy(), tds);
    auto rid = builder_.CreateInBoundsGEP(builder_.getPtrTy(), value, {level});
    value = builder_.CreateLoad(builder_.getPtrTy(), rid);
    cond = builder_.CreateICmpEQ(recTypeIds_[record_t], value);
    builder_.CreateBr(skip);
    builder_.SetInsertPoint(skip);
    auto phi = builder_.CreatePHI(builder_.getInt1Ty(), 2);
    phi->addIncoming(builder_.getFalse(), cur);
    phi->addIncoming(cond, test);
    return phi;
}

Value *LLVMIRBuilder::createTypeTest(Value *value, QualifiedExpression *expr, TypeNode *lType, TypeNode *rType) {
    auto prv = builder_.GetInsertBlock();
    BasicBlock *test, *skip = nullptr;
    if (rType->isPointer()) {
        auto nil = builder_.CreateIsNotNull(value);
        test = BasicBlock::Create(builder_.getContext(), "test", function_);
        skip = BasicBlock::Create(builder_.getContext(), "skip", function_);
        builder_.CreateCondBr(nil, test, skip);
        builder_.SetInsertPoint(test);
    }
    Value *res;
    if (lType->extends(rType)) {
        res = builder_.getTrue();
    } else {
        auto td = getTypeDescriptor(value, expr, rType);
        res = createTypeTest(td, rType);
    }
    if (skip) {
        auto cur= builder_.GetInsertBlock();
        builder_.CreateBr(skip);
        builder_.SetInsertPoint(skip);
        auto phi = builder_.CreatePHI(builder_.getInt1Ty(), 2);
        phi->addIncoming(res, cur);
        phi->addIncoming(builder_.getFalse(), prv);
        return phi;
    }
    return res;
}

void LLVMIRBuilder::createNumericTestCase(CaseOfNode &node, BasicBlock *dflt, BasicBlock *tail) {
    auto *type = dyn_cast<IntegerType>(getLLVMType(node.getExpression()->getType()));
    SwitchInst *inst = builder_.CreateSwitch(value_, dflt, static_cast<unsigned int>(node.getLabelCount()));
    for (size_t i = 0; i < node.getCaseCount(); ++i) {
        auto block = BasicBlock::Create(builder_.getContext(), "case." + to_string(i), function_);
        auto c = node.getCase(i);
        builder_.SetInsertPoint(block);
        c->getStatements()->accept(*this);
        builder_.CreateBr(tail);
        for (int64_t label : c->getLabel()->getValues()) {
            ConstantInt* value = node.getExpression()->getType()->isChar() ?
                                 ConstantInt::get(type, static_cast<uint64_t>(label)) : ConstantInt::getSigned(type, label);
            inst->addCase(value, block);
        }
    }
}

void LLVMIRBuilder::createTypeTestCase(CaseOfNode &node, BasicBlock *dflt, BasicBlock *tail) {
    auto lhs = value_;
    auto lExpr = dynamic_cast<QualifiedExpression *>(node.getExpression());
    auto lType = lExpr->dereference()->getType();
    for (size_t i = 0; i < node.getLabelCount(); ++i) {
        auto c = node.getCase(i);
        auto label = c->getLabel()->getValue(0);
        auto rExpr = dynamic_cast<QualifiedExpression *>(label);
        auto rType = rExpr->dereference()->getType();
        auto name = rType->getIdentifier()->name();
        auto is_true = BasicBlock::Create(builder_.getContext(), "is" + name + "_true", function_);
        auto is_false = BasicBlock::Create(builder_.getContext(), "is" + name + "_false", function_);
        auto cond = createTypeTest(lhs, lExpr, lType, rType);
        builder_.CreateCondBr(cond, is_true, is_false);
        builder_.SetInsertPoint(is_true);
        lExpr->dereference()->setType(rType);   // Set the formal type to the actual type
        c->getStatements()->accept(*this);
        lExpr->dereference()->setType(lType);   // Reset the formal type of the case expression
        if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
            builder_.CreateBr(tail);
        }
        builder_.SetInsertPoint(is_false);
    }
    builder_.CreateBr(dflt);
}

Value *LLVMIRBuilder::createNeg(Value *value) {
    if (config_.isSanitized(Trap::INT_OVERFLOW)) {
        auto type = dyn_cast<IntegerType>(value->getType());
        return trapIntOverflow(Intrinsic::ssub_with_overflow, ConstantInt::get(type, 0), value);
    }
    return builder_.CreateNeg(value);
}

Value *LLVMIRBuilder::createAdd(Value *lhs, Value *rhs) {
    bool sanitize = config_.isSanitized(Trap::INT_OVERFLOW);
    return sanitize ? trapIntOverflow(Intrinsic::sadd_with_overflow, lhs, rhs) : builder_.CreateAdd(lhs, rhs);
}

Value *LLVMIRBuilder::createSub(Value *lhs, llvm::Value *rhs) {
    bool sanitize = config_.isSanitized(Trap::INT_OVERFLOW);
    return sanitize ? trapIntOverflow(Intrinsic::ssub_with_overflow, lhs, rhs) : builder_.CreateSub(lhs, rhs);
}

Value *LLVMIRBuilder::createMul(Value *lhs, llvm::Value *rhs) {
    bool sanitize = config_.isSanitized(Trap::INT_OVERFLOW);
    return sanitize ? trapIntOverflow(Intrinsic::smul_with_overflow, lhs, rhs) : builder_.CreateMul(lhs, rhs);
}

Value *LLVMIRBuilder::createDiv(Value *lhs, Value *rhs) {
    if (config_.isSanitized(Trap::INT_DIVISION)) {
        trapIntDivByZero(rhs);
    }
    Value *div = builder_.CreateSDiv(lhs, rhs);
    value_ = builder_.CreateMul(div, rhs);
    value_ = builder_.CreateSub(lhs, value_);
    Value *cmp = builder_.CreateICmpEQ(value_, ConstantInt::get(lhs->getType(), 0));
    value_ = builder_.CreateXor(lhs, rhs);
    value_ = builder_.CreateAShr(value_, ConstantInt::get(lhs->getType(), lhs->getType()->getIntegerBitWidth() - 1));
    value_ = builder_.CreateSelect(cmp, ConstantInt::get(lhs->getType(), 0), value_);
    return builder_.CreateAdd(value_, div);
}

Value *LLVMIRBuilder::createMod(Value *lhs, Value *rhs) {
    if (config_.isSanitized(Trap::INT_DIVISION)) {
        trapIntDivByZero(rhs);
    }
    Value *rem = builder_.CreateSRem(lhs, rhs);
    value_ = builder_.CreateICmpSLT(rem, ConstantInt::get(lhs->getType(), 0));
    value_ = builder_.CreateSelect(value_, rhs, ConstantInt::get(lhs->getType(), 0));
    return builder_.CreateAdd(value_, rem, "", false, true);
}

Value *LLVMIRBuilder::createFDiv(Value *lhs, Value *rhs) {
    if (config_.isSanitized(Trap::FLT_DIVISION)) {
        trapFltDivByZero(rhs);
    }
    return builder_.CreateFDiv(lhs, rhs);
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
                value_ = type->isReal() ? builder_.CreateFNeg(value_) : createNeg(value_);
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
    // Logical operators `&` and `OR` are treated explicitly in order to enable short-circuiting.
    if (node.getOperator() == OperatorType::AND || node.getOperator() == OperatorType::OR) {
        // Create one block to evaluate the right-hand side and one to skip it.
        auto eval = BasicBlock::Create(builder_.getContext(), "eval", function_);
        auto skip = BasicBlock::Create(builder_.getContext(), "skip", function_);
        // Insert branch to decide whether to skip `&` or `OR`
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
    } else if (node.getOperator() == OperatorType::IS) {
        auto lExpr = dynamic_cast<QualifiedExpression *>(node.getLeftExpression());
        auto lType = lExpr->getType();
        auto rExpr = dynamic_cast<QualifiedExpression *>(node.getRightExpression());
        auto rType = rExpr->dereference()->getType();
        value_ = createTypeTest(lhs, lExpr, lType, rType);
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
        auto rhs = value_;
        // TODO This code assumes that the smallest legal integer is 32 bit.
        if (node.getLeftExpression()->getType()->isInteger() &&
            lhs->getType()->isIntegerTy() && lhs->getType()->getIntegerBitWidth() < 32) {
            lhs = builder_.CreateSExt(lhs, builder_.getInt32Ty());
        }
        if (node.getRightExpression()->getType()->isInteger() &&
            rhs->getType()->isIntegerTy() && rhs->getType()->getIntegerBitWidth() < 32) {
            rhs = builder_.CreateSExt(rhs, builder_.getInt32Ty());
        }
        bool floating = node.getLeftExpression()->getType()->isReal() ||
                        node.getRightExpression()->getType()->isReal();
        switch (node.getOperator()) {
            case OperatorType::PLUS:
                value_ = floating ? builder_.CreateFAdd(lhs, rhs) : createAdd(lhs, rhs);
                break;
            case OperatorType::MINUS:
                value_ = floating ? builder_.CreateFSub(lhs, rhs) : createSub(lhs, rhs);
                break;
            case OperatorType::TIMES:
                value_ = floating ? builder_.CreateFMul(lhs, rhs) : createMul(lhs, rhs);
                break;
            case OperatorType::DIVIDE:
                value_ = createFDiv(lhs, rhs);
                break;
            case OperatorType::DIV: {
                value_ = createDiv(lhs, rhs);
                break;
            }
            case OperatorType::MOD: {
                value_ = createMod(lhs, rhs);
                break;
            }
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
            case OperatorType::IS:
            case OperatorType::AND:
            case OperatorType::OR:
                // Unreachable code due to the if-branch and elsif-branch of this else-branch
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

void LLVMIRBuilder::visit(TypeDeclarationNode &node) {
    string name = node.getIdentifier()->name();
    scopes_.push_back(name);
    getLLVMType(node.getType());
    scopes_.pop_back();
}

void LLVMIRBuilder::visit(ArrayTypeNode &node) {
    types_[&node] = ArrayType::get(getLLVMType(node.types()[0]), node.lengths()[0]);
    // If necessary, create dope vector
    if (node.getBase() == &node && !node.isOpen()) {
        vector<Constant *> dims;
        for (auto len: node.lengths()) {
            dims.push_back(ConstantInt::get(builder_.getInt64Ty(), len));
        }
        auto dvType = ArrayType::get(builder_.getInt64Ty(), node.dimensions());
        auto init = ConstantArray::get(dvType, dims);
        string id = node.getIdentifier() ? node.getIdentifier()->name() : "";
        auto dv = new GlobalVariable(*module_, dvType, true, GlobalValue::InternalLinkage, init, id + ".dv");
        dv->setAlignment(getLLVMAlign(&node));
        typeDopes_[&node] = dv;
    }
}

void LLVMIRBuilder::visit(BasicTypeNode &node) {
    switch (node.kind()) {
        case TypeKind::BOOLEAN:
            types_[&node] = builder_.getInt1Ty(); break;
        case TypeKind::BYTE:
        case TypeKind::CHAR:
            types_[&node] = builder_.getInt8Ty(); break;
        case TypeKind::SHORTINT:
            types_[&node] = builder_.getInt16Ty(); break;
        case TypeKind::INTEGER:
            types_[&node] = builder_.getInt32Ty(); break;
        case TypeKind::LONGINT:
            types_[&node] = builder_.getInt64Ty(); break;
        case TypeKind::REAL:
            types_[&node] = builder_.getFloatTy(); break;
        case TypeKind::LONGREAL:
            types_[&node] = builder_.getDoubleTy(); break;
        case TypeKind::STRING:
            types_[&node] = builder_.getPtrTy(); break;
        case TypeKind::SET:
            types_[&node] = builder_.getInt32Ty(); break;
        default:
            logger_.error(node.pos(), "cannot map type" + to_string(node.kind()) + " to LLVM intermediate representation.");
            types_[&node] = builder_.getVoidTy();
    }
}

void LLVMIRBuilder::visit(ProcedureTypeNode &node) {
    logger_.error(node.pos(), "cannot map type" + to_string(node.kind()) + " to LLVM intermediate representation.");
    types_[&node] = builder_.getVoidTy();
}

void LLVMIRBuilder::visit(RecordTypeNode &node) {
    // Create an empty struct and add it to the lookup table immediately to support recursive records
    auto structTy = StructType::create(builder_.getContext());
    string name = createScopedName(&node);
    structTy->setName("record." + name);
    types_[&node] = structTy;
    vector<Type *> elemTys;
    // Add field for base record
    if (node.isExtended()) {
        elemTys.push_back(getLLVMType(node.getBaseType()));
    }
    // Add regular record fields
    for (size_t i = 0; i < node.getFieldCount(); i++) {
        auto fieldTy = node.getField(i)->getType();
        elemTys.push_back(getLLVMType(fieldTy));
    }
    structTy->setBody(elemTys);
    if (node.getModule() == ast_->getTranslationUnit()) {
        name = module_->getModuleIdentifier() + "__" + name;
        // Create an id for the record type by defining a global int32 variable and using its address
        auto id = new GlobalVariable(*module_, builder_.getInt32Ty(), true, GlobalValue::ExternalLinkage,
                                     Constant::getNullValue(builder_.getInt32Ty()), name + "_id");
        id->setAlignment(module_->getDataLayout().getPreferredAlign(id));
        recTypeIds_[&node] = id;
        // Collect the ids of the record types from which this type is extended
        vector<Constant *> typeIds;
        auto cur = &node;
        Constant *nil = dyn_cast<Constant>(ConstantPointerNull::get(builder_.getPtrTy()));
        while (cur->isExtended()) {
            typeIds.insert(typeIds.begin(), recTypeIds_[cur] ? recTypeIds_[cur] : nil);
            cur = cur->getBaseType();
        }
        typeIds.insert(typeIds.begin(), recTypeIds_[cur] ? recTypeIds_[cur] : nil);
        auto idsType = ArrayType::get(builder_.getPtrTy(), typeIds.size());
        auto ids = new GlobalVariable(*module_, idsType, true, GlobalValue::ExternalLinkage,
                                      ConstantArray::get(idsType, typeIds), name + "_ids");
        ids->setAlignment(module_->getDataLayout().getPreferredAlign(ids));
        // Create the type descriptor
        auto td = new GlobalVariable(*module_, recordTdTy_, true, GlobalValue::ExternalLinkage,
                                     ConstantStruct::get(recordTdTy_, {ids, ConstantInt::get(builder_.getInt32Ty(),
                                                                                             node.getLevel())}),
                                     name + "_td");
        td->setAlignment(module_->getDataLayout().getPreferredAlign(td));
        recTypeTds_[&node] = td;
    } else if (!node.isAnonymous()){
        name = node.getModule()->getIdentifier()->name() + "__" + name;
        // Create an external constant to be used as the id of the imported record type
        auto id = new GlobalVariable(*module_, builder_.getInt32Ty(), true, GlobalValue::ExternalLinkage, nullptr,
                                     name + "_id");
        id->setAlignment(module_->getDataLayout().getPreferredAlign(id));
        recTypeIds_[&node] = id;
        // Create an external constant to be used as the type descriptor of the imported record type
        auto td = new GlobalVariable(*module_, recordTdTy_, true, GlobalValue::ExternalLinkage, nullptr,
                                     name + "_td");
        td->setAlignment(module_->getDataLayout().getPreferredAlign(td));
        recTypeTds_[&node] = td;
    }
}

void LLVMIRBuilder::visit(PointerTypeNode &node) {
    auto base = node.getBase();
    auto type = getLLVMType(base);
    if (base->isRecord()) {
        auto ptrType = ptrTypes_[&node];
        if (!ptrType) {
            ptrType = StructType::create(builder_.getContext(), {builder_.getPtrTy(), type});
            ptrType->setName("record." + createScopedName(&node) + ".ptr");
            ptrTypes_[&node] = ptrType;
        }
        types_[&node] = PointerType::get(ptrType, 0);
    } else {
        types_[&node] = PointerType::get(type, 0);
    }
}

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
    auto rType = node.getRvalue()->getType();
    auto lType = node.getLvalue()->getType();
    setRefMode(!rType->isStructured() && !rType->isString());
    node.getRvalue()->accept(*this);
    cast(*node.getRvalue());
    Value* rValue = value_;
    restoreRefMode();
    node.getLvalue()->accept(*this);
    Value* lValue = value_;
    if (rType->isArray()) {
        auto lArray = dynamic_cast<ArrayTypeNode *>(lType);
        auto rArray = dynamic_cast<ArrayTypeNode *>(rType);
        auto lLen = lArray->isOpen() ? 0 : lArray->lengths()[0];
        auto rLen = rArray->isOpen() ? 0 : rArray->lengths()[0];
        auto len = std::min(lLen, rLen);
        auto layout = module_->getDataLayout();
        auto elemSize = layout.getTypeAllocSize(getLLVMType(lArray->getMemberType()));
        Value *size;
        if (len == 0) {
            setRefMode(true);
            Value *lSize = getArrayLength(node.getLvalue(), 0);
            Value *rSize = getArrayLength(node.getRvalue(), 0);
            restoreRefMode();
            if (config_.isSanitized(Trap::COPY_OVERFLOW)) {
                trapCopyOverflow(lSize, rSize);
            }
            size = builder_.CreateIntrinsic(Intrinsic::umin, {builder_.getInt64Ty()}, {lSize, rSize});
            size = builder_.CreateMul(size, builder_.getInt64(elemSize));
        } else {
            size = builder_.getInt64(len * elemSize);
        }
        value_ = builder_.CreateMemCpy(lValue, {}, rValue, {}, size);
    } else if (rType->isRecord()) {
        auto layout = module_->getDataLayout();
        auto size = layout.getTypeAllocSize(getLLVMType(lType));
        value_ = builder_.CreateMemCpy(lValue, {}, rValue, {}, builder_.getInt64(size));
    } else if (rType->isString()) {
        auto str = dynamic_cast<StringLiteralNode *>(node.getRvalue());
        Value *len = builder_.getInt64(str->value().size() + 1);
        value_ = builder_.CreateMemCpy(lValue, {}, rValue, {}, len);
    } else {
        value_ = builder_.CreateStore(rValue, lValue);
    }
}

void LLVMIRBuilder::visit(CaseOfNode& node) {
    auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
    auto dflt = tail;
    if (node.hasElse()) {
        dflt = BasicBlock::Create(builder_.getContext(), "default", function_);
    }
    setRefMode(true);
    node.getExpression()->accept(*this);
    restoreRefMode();
    // Check whether the case statement is a numeric or a type test
    auto type = node.getExpression()->getType();
    if (type->isInteger() || type->isChar()) {
        createNumericTestCase(node, dflt, tail);
    } else {
        createTypeTestCase(node, dflt, tail);
    }
    if (node.hasElse()) {
        builder_.SetInsertPoint(dflt);
        node.getElseStatements()->accept(*this);
        builder_.CreateBr(tail);
    }
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(CaseLabelNode&) {}

void LLVMIRBuilder::visit(CaseNode&) {}

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
    // Initialize loop counter
    setRefMode(true);
    node.getLow()->accept(*this);
    auto start = value_;
    restoreRefMode();
    node.getCounter()->accept(*this);
    auto counter = value_;
    value_ = builder_.CreateStore(start, counter);
    // Check whether to skip loop body
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
    // Loop body
    builder_.SetInsertPoint(body);
    node.getStatements()->accept(*this);
    // Update loop counter
    setRefMode(true);
    node.getCounter()->accept(*this);
    restoreRefMode();
    counter = builder_.CreateAdd(value_, ConstantInt::getSigned(builder_.getInt32Ty(), step));
    node.getCounter()->accept(*this);
    auto lValue = value_;
    builder_.CreateStore(counter, lValue);
    // Check whether to exit loop body
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
    // After loop
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

void LLVMIRBuilder::visit(ExitNode &) {}

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
        } else if (source->isArray() && target->isString()) {
            auto type = dynamic_cast<ArrayTypeNode*>(source);
            if (type->getMemberType()->kind() == TypeKind::CHAR) {
                value_->getType();
            } else {
                logger_.error(node.pos(), "cannot cast " + to_string(*source->getIdentifier()) + " to " +
                                           to_string(*target->getIdentifier()) + ".");
            }
        }
    }
}

Value *
LLVMIRBuilder::createPredefinedCall(PredefinedProcedure *proc, QualIdent *ident,
                                    vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    ProcKind kind = proc->getKind();
    switch (kind) {
        case ProcKind::NEW:
            return createNewCall(actuals[0]->getType(), params[0]);
        case ProcKind::DISPOSE:
            return createDisposeCall(actuals[0]->getType(), params[0]);
        case ProcKind::INC:
        case ProcKind::DEC:
            return createIncDecCall(kind, actuals, params);
        case ProcKind::LSL:
            return createLslCall(actuals, params);
        case ProcKind::ASR:
            return createAsrCall(actuals, params);
        case ProcKind::ROR:
            return createRorCall(actuals, params);
        case ProcKind::ODD:
            return createOddCall(params[0]);
        case ProcKind::HALT:
            return createExitCall(params[0]);
        case ProcKind::ASSERT:
            return createAssertCall(params[0]);
        case ProcKind::LEN:
            return createLenCall(actuals, params);
        case ProcKind::INCL:
            return createInclCall(params[0], params[1]);
        case ProcKind::EXCL:
            return createExclCall(params[0], params[1]);
        case ProcKind::ORD:
            return createOrdCall(actuals[0].get(), params[0]);
        case ProcKind::CHR:
            return createChrCall(params[0]);
        case ProcKind::SHORT:
            return createShortCall(actuals[0].get(), params[0]);
        case ProcKind::LONG:
            return createLongCall(actuals[0].get(), params[0]);
        case ProcKind::ENTIER:
            return createEntireCall(params[0]);
        case ProcKind::FLT:
            return createFltCall(params[0]);
        case ProcKind::PACK:
            return createPackCall(params[0], params[1]);
        case ProcKind::UNPK:
            return createUnpkCall(actuals, params);
        case ProcKind::ABS:
            return createAbsCall(actuals[0]->getType(), params[0]);
        case ProcKind::MAX:
            return createMaxMinCall(actuals[0].get(), true);
        case ProcKind::MIN:
            return createMaxMinCall(actuals[0].get(), false);
        case ProcKind::SIZE:
        case ProcKind::SYSTEM_SIZE:
            return createSizeCall(actuals[0].get());
        case ProcKind::SYSTEM_ADR:
            return createSystemAdrCall(actuals, params);
        case ProcKind::SYSTEM_GET:
            return createSystemGetCall(actuals, params);
        case ProcKind::SYSTEM_PUT:
            return createSystemPutCall(actuals, params);
        case ProcKind::SYSTEM_BIT:
            return createSystemBitCall(actuals, params);
        case ProcKind::SYSTEM_COPY:
            return createSystemCopyCall(params[0], params[1], params[2]);
        case ProcKind::SYSTEM_VAL:
            return createSystemValCall(actuals, params);
        default:
            logger_.error(ident->start(), "unsupported predefined procedure: " + to_string(*ident) + ".");
            // To generate correct LLVM IR, the current value is returned (no-op).
            return value_;
    }
}

Value *
LLVMIRBuilder::createAbortCall() {
    auto fun = module_->getFunction("abort");
    if (!fun) {
        auto funTy = FunctionType::get(builder_.getVoidTy(), {}, false);
        fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "abort", module_);
        fun->addFnAttr(Attribute::Cold);
        fun->addFnAttr(Attribute::NoReturn);
    }
    builder_.CreateCall(FunctionCallee(fun), {});
    return builder_.CreateUnreachable();
}

Value *
LLVMIRBuilder::createAbsCall(TypeNode *type, llvm::Value *param) {
    if (type->isInteger()) {
        value_ = builder_.CreateIntrinsic(Intrinsic::abs, {param->getType()}, {param, builder_.getInt1(true)});
    } else {
        value_ = builder_.CreateIntrinsic(Intrinsic::fabs, {param->getType()}, {param});
    }
    return value_;
}

Value *
LLVMIRBuilder::createAsrCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
     // TODO : Check for negative shift value with variable argument.
    auto param1 = actuals[1].get();
    if (param1->isLiteral()) {
        // Range check if literal argument
        auto val = dynamic_cast<IntegerLiteralNode *>(param1)->value();
        if (val < 0) {
            logger_.error(param1->pos(), "negative shift value undefined operation.");
            return value_;
        }
    }
    auto shift = builder_.CreateTrunc(params[1], params[0]->getType());
    return builder_.CreateAShr(params[0], shift);
}

Value *
LLVMIRBuilder::createAssertCall(Value *param) {
    if (config_.isSanitized(Trap::ASSERT)) {
        trapAssert(param);
    } else {
        auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
        auto abort = BasicBlock::Create(builder_.getContext(), "abort", function_);
        builder_.CreateCondBr(param, tail, abort);
        builder_.SetInsertPoint(abort);
        value_ = createAbortCall();
        builder_.SetInsertPoint(tail);
    }
    return value_;
}

Value *
LLVMIRBuilder::createChrCall(llvm::Value *param) {
    value_ = builder_.CreateTrunc(param, builder_.getInt8Ty());
    return value_;
}

Value *
LLVMIRBuilder::createEntireCall(llvm::Value *param) {
    Value *value = builder_.CreateIntrinsic(Intrinsic::floor, {param->getType()}, {param});
    value_ = builder_.CreateFPToSI(value, builder_.getInt64Ty());
    return value_;
}

Value *
LLVMIRBuilder::createFltCall(llvm::Value *param) {
    value_ = builder_.CreateSIToFP(param, builder_.getDoubleTy());
    return value_;
}

Value *
LLVMIRBuilder::createPackCall(Value *x, Value *n) {
    auto y = ConstantFP::get(x->getType(), 2.0);
    Value *nval;
    if (n->getType()->getIntegerBitWidth() > 32) {
        nval = builder_.CreateTrunc(n, builder_.getInt32Ty());
    } else if (n->getType()->getIntegerBitWidth() < 32) {
        nval = builder_.CreateSExt(n, builder_.getInt32Ty());
    } else {
        nval = n;
    }
    value_ = builder_.CreateIntrinsic(Intrinsic::powi, {y->getType(), builder_.getInt32Ty()}, {y, nval}); // Usually only Int32 supported by targets
    return builder_.CreateFMul(x, value_);
}

Value *
LLVMIRBuilder::createUnpkCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    auto xtype = getLLVMType(actuals[0]->getType());
    Value *xval = builder_.CreateLoad(xtype, params[0]);
    if (xtype->isFloatTy()) {
        xval = builder_.CreateFPExt(xval, builder_.getDoubleTy());
    }
    auto ret = builder_.CreateIntrinsic(Intrinsic::frexp, {builder_.getDoubleTy(), builder_.getInt32Ty()}, xval); // Usually only Double supported by targets
    auto y = ConstantFP::get(builder_.getDoubleTy(), 2.0);
    xval = builder_.CreateFMul(y, builder_.CreateExtractValue(ret, {0}));
    if (xtype->isFloatTy()) {
        xval = builder_.CreateFPTrunc(xval, builder_.getFloatTy());  
    }
    builder_.CreateStore(xval, params[0]);
    auto ntype = getLLVMType(actuals[1]->getType());
    auto z = ConstantInt::get(ntype, 1);
    auto nval = builder_.CreateExtractValue(ret, {1});
    if (ntype->getIntegerBitWidth() > 32) {
        nval = builder_.CreateTrunc(nval, builder_.getInt32Ty());
    } else if (ntype->getIntegerBitWidth() < 32) {
        nval = builder_.CreateSExt(nval, builder_.getInt32Ty());
    }
    builder_.CreateStore(builder_.CreateSub(nval, z), params[1]);
    return value_;
}

Value *
LLVMIRBuilder::createExitCall(Value *param) {
    auto fun = module_->getFunction("exit");
    if (!fun) {
        auto funTy = FunctionType::get(builder_.getVoidTy(), { builder_.getInt32Ty() }, false);
        fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "exit", module_);
        fun->addFnAttr(Attribute::NoReturn);
        fun->addParamAttr(0, Attribute::NoUndef);
    }
    return builder_.CreateCall(FunctionCallee(fun), { param });
    // TODO The code following exit should be marked as unreachable but this requires more analysis
    // return builder_.CreateUnreachable();
}

Value *
LLVMIRBuilder::createExclCall(Value *set, Value *element) {
    Value *value = builder_.CreateShl(ConstantInt::get(builder_.getInt32Ty(), 0x1), element);
    value = builder_.CreateXor(ConstantInt::get(builder_.getInt32Ty(), 0xffffffff), value);
    value_ = builder_.CreateLoad(builder_.getInt32Ty(), set);
    value_ = builder_.CreateAnd(value_, value);
    return builder_.CreateStore(value_, set);
}

Value *
LLVMIRBuilder::createDisposeCall([[maybe_unused]] TypeNode *type, Value *param) {
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
    values.push_back(builder_.CreateLoad(voidTy, param));
    builder_.CreateCall(FunctionCallee(fun), values);
#ifdef _LLVM_LEGACY
    voidTy = dynamic_cast<PointerType *>(getLLVMType(type));
#endif
    return builder_.CreateStore(ConstantPointerNull::get(voidTy), param);
}

Value *
LLVMIRBuilder::createIncDecCall(ProcKind kind,
                                vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    auto target = getLLVMType(actuals[0]->getType());
    Value *delta;
    if (params.size() > 2) {
        auto param = actuals[2].get();
        logger_.error(param->pos(), "more actual than formal parameters.");
        return value_;
    } else if (params.size() > 1) {
        auto param0 = actuals[0].get();
        auto param1 = actuals[1].get();
        if (!param1->getType()->isInteger()) {
            logger_.error(param1->pos(), "type mismatch: expected integer type, found " +
                                         to_string(param1->getType()) + ".");
            return value_;
        }
        auto source = params[1]->getType();
        if (target->getIntegerBitWidth() > source->getIntegerBitWidth()) {
            delta = builder_.CreateSExt(params[1], target);
        } else if (target->getIntegerBitWidth() < source->getIntegerBitWidth()) {
            logger_.warning(param1->pos(), "type mismatch: truncating " + to_string(param1->getType())
                                           + " to " + to_string(param0->getType()) + " may lose data.");
            delta = builder_.CreateTrunc(params[1], target);
        } else {
            delta = params[1];
        }
    } else {
        delta = ConstantInt::get(target, 1);
    }
    Value *value = builder_.CreateLoad(target, params[0]);
    if (kind == ProcKind::INC) {
        value = createAdd(value, delta);
    } else {
        value = createSub(value, delta);
    }
    return builder_.CreateStore(value, params[0]);
}

Value *
LLVMIRBuilder::createInclCall(llvm::Value *set, llvm::Value *element) {
    Value *value = builder_.CreateShl(ConstantInt::get(builder_.getInt32Ty(), 0x1), element);
    value_ = builder_.CreateLoad(builder_.getInt32Ty(), set);
    value_ = builder_.CreateOr(value_, value);
    return builder_.CreateStore(value_, set);
}

Value *
LLVMIRBuilder::createLenCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    // `params[0]`: the array for which the length will be returned
    // `params[1]`: pointer to the dope vector of the array
    // `params[2]`: the (optional) dimension of the array
    auto param0 = actuals[0].get();
    if (param0->getType()->isString() && param0->isLiteral()) {
        auto str = dynamic_cast<StringLiteralNode *>(param0);
        value_ = builder_.getInt64(str->value().size() + 1);
        return value_;
    }
    auto array_t = dynamic_cast<ArrayTypeNode *>(param0->getType());
    int64_t dim = 0;
    if (params.size() > 3) {
        auto param = actuals[2].get();
        logger_.error(param->pos(), "more actual than formal parameters.");
        return value_;
    } else if (params.size() > 2) {
        auto param1 = actuals[1].get();
        if (!param1->getType()->isInteger()) {
            logger_.error(param1->pos(), "type mismatch: expected integer type, found "
                                         + to_string(param1->getType()) + ".");
            return value_;
        }
        if (param1->isLiteral()) {
            dim = dynamic_cast<IntegerLiteralNode *>(param1)->value();
            if (dim < 0) {
                logger_.error(param1->pos(), "array dimension cannot be a negative value.");
                return value_;
            }
            if (static_cast<uint32_t>(dim) >= array_t->dimensions()) {
                logger_.error(param1->pos(), "value exceeds number of array dimensions.");
                return value_;
            }
        } else {
            logger_.error(param1->pos(), "constant expression expected.");
            return value_;
        }
    }
    if (!array_t->isOpen()) {
        value_ = builder_.getInt64(array_t->lengths()[(size_t) dim]);
        return value_;
    }
    return getOpenArrayLength(params[1], array_t, static_cast<uint32_t>(dim));
}

Value *
LLVMIRBuilder::createLongCall(ExpressionNode *expr, llvm::Value *param) {
    auto type = expr->getType();
    if (type->isInteger()) {
        if (type->kind() == TypeKind::SHORTINT) {
            value_ = builder_.CreateSExt(param, builder_.getInt32Ty());
        } else if (type->kind() == TypeKind::INTEGER) {
            value_ = builder_.CreateSExt(param, builder_.getInt64Ty());
        } else {
            logger_.error(expr->pos(), "illegal integer expression.");
        }
    } else {
        if (type->kind() == TypeKind::REAL) {
            value_ = builder_.CreateFPExt(param, builder_.getDoubleTy());
        } else {
            logger_.error(expr->pos(), "illegal floating-point expression.");
        }
    }
    return value_;
}

Value *
LLVMIRBuilder::createLslCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    // TODO Check for negative shift value with variable argument.
    auto param1 = actuals[1].get();
    if (param1->isLiteral()) {
        // Range check if literal argument
        auto val = dynamic_cast<IntegerLiteralNode *>(param1)->value();
        if (val < 0) {
            logger_.error(param1->pos(), "negative shift value undefined.");
            return value_;
        }
    }
    auto shift = builder_.CreateTrunc(params[1], params[0]->getType());
    return builder_.CreateShl(params[0], shift);
}

Value *
LLVMIRBuilder::createMaxMinCall(ExpressionNode *actual, bool isMax) {
    auto decl = dynamic_cast<QualifiedExpression *>(actual)->dereference();
    auto type = dynamic_cast<TypeDeclarationNode *>(decl)->getType();
    if (type->isReal()) {
        if (type->getSize() == 4) {
            value_ = ConstantFP::getInfinity(builder_.getFloatTy(), !isMax);
        } else {
            value_ = ConstantFP::getInfinity(builder_.getDoubleTy(), !isMax);
        }
    } else if (type->isInteger()) {
        if (type->getSize() == 8) {
            if (isMax) {
                value_ = builder_.getInt64((uint64_t)std::numeric_limits<int64_t>::max());
            } else {
                value_ = builder_.getInt64((uint64_t)std::numeric_limits<int64_t>::min());
            }
        } else if (type->getSize() == 4) {
            if (isMax) {
                value_ = builder_.getInt32((uint32_t)std::numeric_limits<int32_t>::max());
            } else {
                value_ = builder_.getInt32((uint32_t)std::numeric_limits<int32_t>::min());
            }
        } else {
            if (isMax) {
                value_ = builder_.getInt16((uint16_t)std::numeric_limits<int16_t>::max());
            } else {
                value_ = builder_.getInt16((uint16_t)std::numeric_limits<int16_t>::min());
            }
        }
    } else {
        logger_.error(actual->pos(), "type mismatch: REAL or INTEGER expected.");
    }
    return value_;
}

Value *
LLVMIRBuilder::createNewCall(TypeNode *type, llvm::Value *param) {
    auto fun = module_->getFunction("malloc");
    if (!fun) {
        auto funTy = FunctionType::get(builder_.getPtrTy(), { builder_.getInt64Ty() }, false);
        fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "malloc", module_);
        fun->addFnAttr(Attribute::getWithAllocSizeArgs(builder_.getContext(), 0, {}));
        fun->addParamAttr(0, Attribute::NoUndef);
    }
    std::vector<Value *> values;
    auto ptr = dynamic_cast<PointerTypeNode *>(type);
    auto layout = module_->getDataLayout();
    auto base = ptr->getBase();
    // TODO Move the following distinction to `getLLVMType`?
    if (base->isRecord()) {
        values.push_back(ConstantInt::get(builder_.getInt64Ty(), layout.getTypeAllocSize(ptrTypes_[ptr])));
    } else {
        values.push_back(ConstantInt::get(builder_.getInt64Ty(), layout.getTypeAllocSize(getLLVMType(base))));
    }
    value_ = builder_.CreateCall(FunctionCallee(fun), values);
    // TODO Remove next line (bit-cast) once non-opaque pointers are no longer supported
    Value *value = builder_.CreateBitCast(value_, getLLVMType(ptr));
    builder_.CreateStore(value, param);
    if (base->isRecord()) {
        Value *id = recTypeTds_[dynamic_cast<RecordTypeNode *>(base)];
        value = builder_.CreateLoad(builder_.getPtrTy(), param);
        value = builder_.CreateInBoundsGEP(ptrTypes_[ptr], value, {builder_.getInt32(0), builder_.getInt32(0)});
        builder_.CreateStore(id, value);
    }
    return value_;
}

Value *
LLVMIRBuilder::createOddCall(llvm::Value *param) {
    auto paramTy = param->getType();
    value_ = builder_.CreateAnd(param, ConstantInt::get(paramTy, 1));
    return builder_.CreateICmpEQ(value_, ConstantInt::get(paramTy, 1));
}

Value *
LLVMIRBuilder::createOrdCall(ExpressionNode *actual, llvm::Value *param) {
    if (actual->getType()->isBoolean()) {
        Value *value = builder_.CreateAlloca(builder_.getInt32Ty());
        auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
        auto return1 = BasicBlock::Create(builder_.getContext(), "true", function_);
        auto return0 = BasicBlock::Create(builder_.getContext(), "false", function_);
        builder_.CreateCondBr(param, return1, return0);
        builder_.SetInsertPoint(return1);
        builder_.CreateStore(ConstantInt::get(builder_.getInt32Ty(), 0x1), value);
        builder_.CreateBr(tail);
        builder_.SetInsertPoint(return0);
        builder_.CreateStore(ConstantInt::get(builder_.getInt32Ty(), 0x0), value);
        builder_.CreateBr(tail);
        builder_.SetInsertPoint(tail);
        value_ = builder_.CreateLoad(builder_.getInt32Ty(), value);
    } else if (actual->getType()->isChar()) {
        value_ = builder_.CreateZExt(param, builder_.getInt32Ty());
    } else if (actual->getType()->isSet()) {
        value_ = param;
    } else {
        logger_.error(actual->pos(), "type mismatch: BOOLEAN, CHAR, or SET expected.");
    }
    return value_;
}

Value *
LLVMIRBuilder::createRorCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    // TODO Check for negative shift value with variable argument.
    auto param1 = actuals[1].get();
    if (param1->isLiteral()) {
        // Range check if literal argument
        auto val = dynamic_cast<IntegerLiteralNode *>(param1)->value();
        if (val < 0) {
            logger_.error(param1->pos(), "negative shift value undefined.");
            return value_;
        }
    }
    auto shift = builder_.CreateTrunc(params[1], params[0]->getType());
    Value *lhs = builder_.CreateLShr(params[0], shift);
    Value *value = ConstantInt::get(params[0]->getType(), params[0]->getType()->getIntegerBitWidth());
    Value *delta = builder_.CreateSub(value, shift);
    Value *rhs = builder_.CreateShl(params[0], delta);
    return builder_.CreateOr(lhs, rhs);
}

Value *
LLVMIRBuilder::createShortCall(ExpressionNode *expr, llvm::Value *param) {
    if (expr->isLiteral()) {
        logger_.error(expr->pos(), "constant not valid parameter.");
        return value_;
    }
    auto type = expr->getType();
    if (type->isInteger()) {
        if (type->kind() == TypeKind::INTEGER) {
            value_ = builder_.CreateTrunc(param, builder_.getInt16Ty());
        } else if (type->kind() == TypeKind::LONGINT) {
            value_ = builder_.CreateTrunc(param, builder_.getInt32Ty());
        } else {
            logger_.error(expr->pos(), "illegal integer expression.");
        }
    } else {
        if (type->kind() == TypeKind::LONGREAL) {
            value_ = builder_.CreateFPTrunc(param, builder_.getFloatTy());
        } else {
            logger_.error(expr->pos(), "illegal floating-point expression.");
        }
    }
    return value_;
}

Value *
LLVMIRBuilder::createSizeCall(ExpressionNode *expr) {
    auto decl = dynamic_cast<QualifiedExpression *>(expr)->dereference();
    auto type = dynamic_cast<TypeDeclarationNode *>(decl);
    auto layout = module_->getDataLayout();
    auto size = layout.getTypeAllocSize(getLLVMType(type->getType()));
    value_ = builder_.getInt64(size);
    return value_;
}

Value *
LLVMIRBuilder::createSystemAdrCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    // TODO Handle procedure reference
    auto actual = actuals[0].get();
    auto type = actual->getType();
    // TODO Isn't CHAR a basic type?
    if (type->isChar() || type->isBasic()) {
        value_ = builder_.CreatePtrToInt(params[0], builder_.getInt64Ty());
    } else if (type->isArray() || type->isString()) {
        auto arrayTy = getLLVMType(type);
        vector<Value *> indices;
        indices.push_back(builder_.getInt32(0));
        if (type->isString() && actual->isLiteral()) {
            auto str = dynamic_cast<StringLiteralNode *>(actual);
            auto stringTy = ArrayType::get(builder_.getInt8Ty(), str->value().size() + 1);
            arrayTy = StructType::get(builder_.getInt64Ty(), stringTy);
            // indices.push_back(builder_.getInt32(1));
            // indices.push_back(builder_.getInt32(0));
        } else {
            auto atype = dynamic_cast<ArrayTypeNode *>(type);
            // TODO Isn't CHAR a basic type?
            if (!atype->getMemberType()->isChar() && !atype->getMemberType()->isBasic()) {
                logger_.error(actual->pos(), "expected array of basic type or CHAR");
                return value_;
            }
        }
        value_ = builder_.CreateInBoundsGEP(arrayTy, params[0], indices);
    } else if (type->isRecord()) {
        auto recordTy = getLLVMType(type);
        vector<Value *> indices;
        indices.push_back(builder_.getInt32(0));
        auto rtype = dynamic_cast<RecordTypeNode *>(type);
        for (size_t i = 0; i < rtype->getFieldCount(); i++) {
            auto fieldTy = rtype->getField(i)->getType();
            // TODO Isn't CHAR a basic type?
            if (!fieldTy->isChar() && !fieldTy->isBasic()) {
                logger_.error(actual->pos(), "expected record of basic type or CHAR");
                return value_;
            }
        }
        value_ = builder_.CreateInBoundsGEP(recordTy, params[0], indices);
    } else {
        logger_.error(actual->pos(), "type missmatch");
    }
    return value_;
}

Value *
LLVMIRBuilder::createSystemGetCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    auto param = actuals[1].get();
    auto type = param->getType();
    if (type->isChar() || type->isBasic()) {
        auto base = getLLVMType(type);
        auto ptrtype = PointerType::get(base, 0);
        auto ptr = builder_.CreateIntToPtr(params[0], ptrtype);
        auto value = builder_.CreateLoad(base, ptr, true);
        value_ = builder_.CreateStore(value, params[1]);
    } else {
        logger_.error(param->pos(), "expected basic or char type");
    }
    return value_;
}

Value *
LLVMIRBuilder::createSystemPutCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    auto param = actuals[1].get();
    auto type = param->getType();
    if (type->isChar() || type->isBasic()) {
        auto base = getLLVMType(type);
        auto ptrtype = PointerType::get(base, 0);
        auto ptr = builder_.CreateIntToPtr(params[0], ptrtype);
        value_ = builder_.CreateStore(params[1], ptr, true);
    } else {
        logger_.error(param->pos(), "expected basic or char type");
    }
    return value_;
}

Value *
LLVMIRBuilder::createSystemBitCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    auto param1 = actuals[1].get();
    if (param1->isLiteral()) {
        auto val = dynamic_cast<IntegerLiteralNode *>(param1)->value();
        if ((val < 0) || (val > 31)) {
            logger_.error(param1->pos(), "bit position between 0 and 31.");
            return value_;
        }
    } else {
        logger_.error(param1->pos(), "constant expression expected.");
        return value_;
    }
    auto base = builder_.getInt32Ty();
    auto ptrtype = PointerType::get(base, 0);
    auto ptr = builder_.CreateIntToPtr(params[0], ptrtype);
    auto value = builder_.CreateLoad(base, ptr, true);
    Value *lhs = builder_.CreateLShr(value, params[1]);
    Value *rhs = ConstantInt::get(base, 0x00000001);
    Value *res = builder_.CreateAnd(lhs, rhs);
    return builder_.CreateICmpEQ(res, ConstantInt::get(base, 1));
}

Value *
LLVMIRBuilder::createSystemCopyCall(llvm::Value *src, llvm::Value *dst, llvm::Value *n) {
    auto ptrtype = builder_.getPtrTy();
    auto srcptr = builder_.CreateIntToPtr(src, ptrtype);
    auto dstptr = builder_.CreateIntToPtr(dst, ptrtype);
    return builder_.CreateMemCpy(dstptr, {}, srcptr, {}, builder_.CreateShl(n, 2), true);
}

Value *
LLVMIRBuilder::createSystemValCall(vector<unique_ptr<ExpressionNode>> &actuals, std::vector<Value *> &params) {
    // TODO Introduce support further types: RECORD, etc.
    auto dst = actuals[0].get();
    auto decl = dynamic_cast<QualifiedExpression *>(dst)->dereference();
    auto dsttype = dynamic_cast<TypeDeclarationNode *>(decl)->getType();
    auto src = actuals[1].get();
    auto srctype = src->getType();
    if (!srctype->isBasic() || !dsttype->isBasic()) {
        logger_.error(dst->pos(), "expected basic type");
        return value_;
    }
    Value *srcpar;
    if (srctype->isReal()) {
        if (srctype->getSize() == 4) {
            srcpar = builder_.CreateBitCast(params[1], builder_.getInt32Ty());
        } else {
            srcpar = builder_.CreateBitCast(params[1], builder_.getInt64Ty());
        }
    } else {
        srcpar = params[1];
    }
    if (dsttype->isReal()) {
        auto rcast = dsttype->getSize() == 4 ? builder_.getInt32Ty() : builder_.getInt64Ty();
        if (srctype->getSize() <= dsttype->getSize()) {
            srcpar = builder_.CreateZExt(srcpar, rcast);
        } else {
            srcpar = builder_.CreateTrunc(srcpar, rcast);
        }
        return builder_.CreateBitCast(srcpar, getLLVMType(dsttype));
    }
    if (srctype->getSize() <= dsttype->getSize()) {
        return builder_.CreateZExt(srcpar, getLLVMType(dsttype));
    } else {
        return builder_.CreateTrunc(srcpar, getLLVMType(dsttype));
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
        auto type = param->getType();
        auto param_t = getLLVMType(type);
        params.push_back(param->isVar() || type->isStructured() ? param_t->getPointerTo() : param_t);
        // Add a parameter for the "dope vector" in case of an open array parameter
        // or for the type descriptor in case of a variable record parameter
        if ((type->isArray() && dynamic_cast<ArrayTypeNode*>(type)->isOpen()) || (type->isRecord() && param->isVar())) {
            params.push_back(builder_.getPtrTy());
        }
    }
    auto type = FunctionType::get(getLLVMType(node.getType()->getReturnType()), params, node.getType()->hasVarArgs());
    auto callee = module_->getOrInsertFunction(name, type);
    auto function = dyn_cast<Function>(callee.getCallee());
    if (triple_.isOSWindows() && node.getIdentifier()->isExported() && !config_.hasFlag(Flag::ENABLE_MAIN)) {
        function->setDLLStorageClass(llvm::GlobalValue::DLLStorageClassTypes::DLLExportStorageClass);
    }
    functions_[&node] = function;
}

string LLVMIRBuilder::qualifiedName(DeclarationNode *node) const {
    if (node->getNodeType() == NodeType::procedure) {
        auto proc = dynamic_cast<ProcedureNode *>(node);
        if ((proc->isExtern() || proc->isImported()) && node->getModule() == ast_->getTranslationUnit()) {
            return node->getIdentifier()->name();
        }
    }
    return node->getModule()->getIdentifier()->name() + "_" + node->getIdentifier()->name();
}

string LLVMIRBuilder::createScopedName(TypeNode *type) const {
    if (!type->isAnonymous() && scopes_.size() <= 1) {
        return type->getIdentifier()->name();
    }
    ostringstream oss;
    string sep = "";
    size_t i = 0;
    while (i < scopes_.size()) {
        oss << sep << scopes_[i++];
        if (!type->isAnonymous() && (i == scopes_.size() - 1)) {
            return oss.str() + "_" + type->getIdentifier()->name();
        }
    }
    return oss.str();
}

Value *LLVMIRBuilder::processGEP(Type *base, Value *value, vector<Value *> &indices) {
    if (indices.size() > 1) {
        auto result = builder_.CreateInBoundsGEP(base, value, indices);
        indices.clear();
        indices.push_back(builder_.getInt32(0));
        return result;
    }
    return value;
}

Type* LLVMIRBuilder::getLLVMType(TypeNode *type) {
    if (type == nullptr) {
        return builder_.getVoidTy();
    } else if (!types_[type]) {
        type->accept(*this);
    }
    return types_[type];
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
