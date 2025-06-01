/*
 * Simple tree-walk code generator to build LLVM IR for the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#include "LLVMIRBuilder.h"

#include <limits>
#include <sstream>
#include <vector>
#include <llvm/IR/Verifier.h>
#include "system/PredefinedProcedure.h"

using std::ostringstream;
using std::vector;

LLVMIRBuilder::LLVMIRBuilder(CompilerConfig &config, LLVMContext &builder, Module *module) :
        NodeVisitor(), config_(config), logger_(config_.logger()), builder_(builder), module_(module),
		triple_(module->getTargetTriple()), value_(), scope_(0), function_(), attrs_(AttrBuilder(builder)) {
    attrs_
            .addAttribute(Attribute::NoInline)
            .addAttribute(Attribute::NoUnwind)
            .addAttribute(Attribute::OptimizeNone)
#ifndef _LLVM_LEGACY
            .addAttribute(Attribute::getWithUWTableKind(builder, UWTableKind::Default))
#endif
            ;
#if !(defined(__MINGW32__) || defined(__MINGW64__))
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
    const string prefix = node.getIdentifier()->name() + "_";
    for (size_t i = 0; i < node.getVariableCount(); ++i) {
        const auto variable = node.getVariable(i);
        const auto type = getLLVMType(variable->getType());
        const bool expo = variable->getIdentifier()->isExported();
        const string name = expo ? prefix + variable->getIdentifier()->name() : variable->getIdentifier()->name();
        const auto value = new GlobalVariable(*module_, type, false,
                                              expo ? GlobalValue::ExternalLinkage : GlobalValue::InternalLinkage,
                                             Constant::getNullValue(type),name);
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
        function_->setDLLStorageClass(GlobalValue::DLLStorageClassTypes::DLLExportStorageClass);
    }
    auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
    builder_.SetInsertPoint(entry);
    scope_ = node.getScope() + 1;
    // Generate code to initialize imports
    for (const auto &import : node.imports()) {
        import->accept(*this);
    }
    // Initialize array sizes
    for (size_t i = 0; i < node.getVariableCount(); ++i) {
        const auto var = node.getVariable(i);
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
    const string name = node.getIdentifier()->name();
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
    const auto entry = BasicBlock::Create(builder_.getContext(), "entry", function_);
    builder_.SetInsertPoint(entry);
    Function::arg_iterator args = function_->arg_begin();
    // Allocate space for parameters
    for (auto &param : node.getType()->parameters()) {
        auto arg = args++;
        arg->addAttr(Attribute::AttrKind::NoUndef);
        // Pass variable and structured parameters, i.e., arrays and records, by reference
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
        const auto variable = node.getVariable(i);
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
    const auto block = builder_.GetInsertBlock();
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
    const string name = node.getModule()->name();
    if (name == "SYSTEM") {
        return; // No initialization for pseudo modules
    }
    const auto type = FunctionType::get(builder_.getInt32Ty(), {});
    if (auto fun = module_->getOrInsertFunction(name, type)) {
        value_ = builder_.CreateCall(fun, {});
    } else {
        logger_.error(node.pos(), "undefined procedure: " + name + ".");
    }
}

void LLVMIRBuilder::visit(QualifiedStatement &node) {
    const auto decl = node.dereference();
    if (decl->getNodeType() == NodeType::procedure) {
        createStaticCall(node, node.ident(), node.selectors());
    } else {
        value_ = values_[decl];
        if (decl->getNodeType() == NodeType::parameter) {
            value_ = builder_.CreateLoad(builder_.getPtrTy(), value_);
        }
        selectors(&node, decl->getType(), node.selectors().begin(), node.selectors().end());
    }
}

void LLVMIRBuilder::visit(QualifiedExpression &node) {
    const auto decl = node.dereference();
    if (decl->getNodeType() == NodeType::type) {
        // If the qualified expression refers to a type, no code has to be generated.
        return;
    }
    const auto scope = decl->getScope();
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
    if (const auto procTy = dynamic_cast<ProcedureTypeNode *>(type)) {
        // Check whether this is a procedure call or a procedure variable.
        if (decl->getNodeType() == NodeType::procedure) {
            if (procTy->getReturnType() == nullptr || node.selectors().empty()) {
                // Looks like a reference to a procedure (no return type) or to a function procedure (no selectors).
                value_ = module_->getFunction(qualifiedName(decl));
            } else {
                createStaticCall(node, node.ident(), node.selectors());
            }
            return;
        }
    }
    if (!value_ && decl->getNodeType() == NodeType::variable) {
        // It looks like we came across a reference to an imported variable.
        const auto variable = dynamic_cast<VariableDeclarationNode *>(decl);
        const auto value = new GlobalVariable(*module_, getLLVMType(variable->getType()), false,
                                              GlobalValue::ExternalLinkage, nullptr, qualifiedName(decl));
        value->setAlignment(module_->getDataLayout().getPreferredAlign(value));
        values_[decl] = value;
        value_ = value;
    }
    if (decl->getNodeType() == NodeType::parameter) {
        const auto param = dynamic_cast<ParameterNode *>(decl);
        // Since variable and structured parameters are passed as pointers
        // for performance reasons, they need to be explicitly dereferenced.
        if (param->isVar() || param->getType()->isStructured()) {
            value_ = builder_.CreateLoad(builder_.getPtrTy(), value_);
        }
    }
    type = selectors(&node, type, node.selectors().begin(), node.selectors().end());
    if (deref()) {
        // Check whether the qualified expression is other than a function procedure call.
        if (node.selectors().empty() || node.selectors().back()->getNodeType() != NodeType::parameter) {
            value_ = builder_.CreateLoad(type->isStructured() ? builder_.getPtrTy() : getLLVMType(type), value_);
        }
    }
}

Value *LLVMIRBuilder::getArrayLength(ExpressionNode *expr, const uint32_t dim) {
    const auto type = dynamic_cast<ArrayTypeNode *>(expr->getType());
    if (type->isOpen()) {
        Value *dopeV = getDopeVector(expr);
        return getOpenArrayLength(dopeV, type, dim);
    }
    return builder_.getInt64(type->lengths()[dim]);
}

Value *LLVMIRBuilder::getOpenArrayLength(Value *dopeV, const ArrayTypeNode *type, const uint32_t dim, const bool ref) {
    const auto dopeTy = ArrayType::get(builder_.getInt64Ty(), type->dimensions());
    if (ref) {
        // dereference the pointer to the dope vector
        dopeV = builder_.CreateLoad(builder_.getPtrTy(), dopeV);
    }
    Value *value = builder_.CreateInBoundsGEP(dopeTy, dopeV, {builder_.getInt32(0), builder_.getInt32(dim) });
    return builder_.CreateLoad(builder_.getInt64Ty(), value);
}

Value *LLVMIRBuilder::getDopeVector(ExpressionNode *expr) {
    if (const auto qual = dynamic_cast<QualifiedExpression *>(expr)) {
        return getDopeVector(qual, qual->getType());
    }
    logger_.error(expr->pos(), "cannot determine array telemetry of expression.");
    return nullptr;
}

Value *LLVMIRBuilder::getDopeVector(const NodeReference *ref, TypeNode *type) {
    Value *dopeV = nullptr;
    const auto decl = ref->dereference();
    if (const auto array_t = dynamic_cast<ArrayTypeNode *>(type)) {
        // Find the base array type of the parameter expression
        const auto base = array_t->getBase();
        if (array_t->isOpen()) {
            dopeV = valueDopes_[decl];
        } else {
            dopeV = typeDopes_[base];
        }
        // Check whether the actual array has fewer dimensions than the base array due to applied array indices
        if (array_t->dimensions() < base->dimensions()) {
            const auto delta = base->dimensions() - array_t->dimensions();
            const auto dopeTy = ArrayType::get(builder_.getInt64Ty(), base->dimensions());
            dopeV = deref() ? builder_.CreateLoad(builder_.getPtrTy(), dopeV) : dopeV;
            dopeV = builder_.CreateInBoundsGEP(dopeTy, dopeV, {builder_.getInt32(0), builder_.getInt32(delta)});
        }
    } else if (decl->getType()->isArray()) {
        dopeV = valueDopes_[decl];
    }
    if (!dopeV) {
        logger_.error(ref->pos(), "cannot determine array telemetry of expression.");
    }
    return dopeV;
}

Value *LLVMIRBuilder::getTypeDescriptor(Value *value, const NodeReference *ref, TypeNode *type) {
    Value *typeD = nullptr;
    const auto decl = ref->dereference();
    if (type->isPointer()) {
        const auto pointer_t = dynamic_cast<PointerTypeNode *>(type);
        typeD = builder_.CreateInBoundsGEP(ptrTypes_[pointer_t], value, {builder_.getInt32(0), builder_.getInt32(0)});
    } else if (decl->getType()->isRecord() && type->isRecord() && type->extends(decl->getType())) {
        typeD = valueTds_[decl];
    }
    if (!typeD) {
        logger_.error(ref->pos(), "cannot determine type description of expression.");
    }
    return typeD;
}

TypeNode *LLVMIRBuilder::selectors(NodeReference *ref, TypeNode *base, const SelectorIterator start, const SelectorIterator end) {
    if (!base || base->isVirtual()) {
        return nullptr;
    }
    auto decl = ref->dereference();
    auto selector_t = base;
    auto baseTy = getLLVMType(selector_t);
    auto value = value_;
    vector<Value *> indices;
    indices.push_back(builder_.getInt32(0));
    for (auto it = start; it != end; ++it) {
        const auto sel = it->get();
        if (sel->getNodeType() == NodeType::parameter) {
            const auto params = dynamic_cast<ActualParameters *>(sel);
            auto procedure_t = dynamic_cast<ProcedureTypeNode *>(selector_t);
            vector<Value *> values;
            parameters(procedure_t, params, values);
            const auto funTy = funTypes_[procedure_t];
            // Output the GEP up to the procedure call
            value = processGEP(baseTy, value, indices);  // TODO Not sure if this call is ever necessary
            // Create a load to dereference the function pointer
            if (decl && (decl->getNodeType() == NodeType::variable ||
                         decl->getNodeType() == NodeType::parameter ||
                         decl->getNodeType() == NodeType::field)) {
                value = builder_.CreateLoad(builder_.getPtrTy(), value);
            }
            if (config_.isSanitized(Trap::NIL_POINTER)) {
                trapNILPtr(value);
            }
            value = builder_.CreateCall(dyn_cast<FunctionType>(funTy), value, values);
            decl = nullptr;
            selector_t = procedure_t->getReturnType();
            baseTy = getLLVMType(selector_t);
        } else if (sel->getNodeType() == NodeType::array_type) {
            const auto array = dynamic_cast<ArrayIndex *>(sel);
            const auto array_t = dynamic_cast<ArrayTypeNode *>(selector_t);
            setRefMode(true);
            Value *dopeV = nullptr;
            for (size_t i = 0; i < array->indices().size(); ++i) {
                const auto index = array->indices()[i].get();
                index->accept(*this);
                if (config_.isSanitized(Trap::OUT_OF_BOUNDS) && (array_t->isOpen() || !index->isLiteral())) {
                    Value *lower = builder_.getInt64(0);
                    Value *upper;
                    if (array_t->isOpen()) {
                        if (!dopeV) {
                            if (const auto expr = dynamic_cast<QualifiedExpression *>(ref)) {
                                dopeV = getDopeVector(expr);
                            } else {
                                dopeV = getDopeVector(ref, array_t);
                            }
                        }
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
            const auto field = dynamic_cast<RecordField *>(sel)->getField();
            const auto record_t = dynamic_cast<RecordTypeNode *>(selector_t);
            // Navigate through the base records
            const unsigned current = field->getRecordType()->getLevel();
            for (unsigned level = current; level < record_t->getLevel(); level++) {
                indices.push_back(builder_.getInt32(0));
            }
            // Access the field by its index (increase index at levels > 0)
            indices.push_back(builder_.getInt32(current == 0 ? field->getIndex() : field->getIndex() + 1));
            // Output GEP up to the record field
            value = processGEP(baseTy, value, indices);
            decl = field;
            selector_t = field->getType();
            baseTy = getLLVMType(selector_t);
        } else if (sel->getNodeType() == NodeType::type) {
            const auto guard = dynamic_cast<Typeguard *>(sel);
            if (config_.isSanitized(Trap::TYPE_GUARD)) {
                const auto guard_t = guard->getType();
                auto tmp = value;
                if (guard_t->isPointer()) {
                    value = processGEP(baseTy, value, indices);
                    tmp = builder_.CreateLoad(builder_.getPtrTy(), value);
                }
                const auto cond = createTypeTest(tmp, ref, selector_t, guard_t);
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
LLVMIRBuilder::parameters(ProcedureTypeNode *proc, ActualParameters *actuals, vector<Value *> &values, bool) {
    for (size_t i = 0; i < actuals->parameters().size(); i++) {
        const auto actualParam = actuals->parameters()[i].get();
        const auto actualType = actualParam->getType();
        const ParameterNode *formalParam = nullptr;
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
            const auto formalType = formalParam->getType();
            if (formalType->isArray() && dynamic_cast<ArrayTypeNode*>(formalType)->isOpen()) {
                // Add a pointer to the dope vector of an open array
                if (actualType->isString() && actualParam->isLiteral()) {
                    // Create a "dope vector" on-the-fly for string literals
                    const auto str = dynamic_cast<StringLiteralNode *>(actualParam);
                    Value *dopeV = builder_.CreateAlloca(ArrayType::get(builder_.getInt64Ty(), 1));
                    string value = str->value();
                    builder_.CreateStore(builder_.getInt64(value[0] == '\0' ? 1 : value.size() + 1), dopeV);
                    values.push_back(dopeV);
                } else {
                    // Look up the "dope vector" of explicitly defined array types
                    Value *dopeV = getDopeVector(actualParam);
                    if (actualType->isArray() && dynamic_cast<ArrayTypeNode *>(actualType)->isOpen()) {
                        // It looks like an open array is passed from one procedure to the next
                        dopeV = builder_.CreateLoad(builder_.getPtrTy(), dopeV);
                    }
                    values.push_back(dopeV);
                }
            } else if (formalType->isRecord() && formalParam->isVar()) {
                // Add a pointer to the type descriptor of the variable record
                values.push_back(recTypeTds_[dynamic_cast<RecordTypeNode *>(actualType)]);
            }
        }
    }
}

TypeNode *LLVMIRBuilder::createStaticCall(NodeReference &node, const QualIdent *ident, Selectors &selectors) {
    const auto proc = dynamic_cast<ProcedureNode *>(node.dereference());
    const auto type = proc->getType();
    vector<Value*> values;
    ActualParameters *sel;
    bool args;
    if (selectors.empty() || selectors[0].get()->getNodeType() != NodeType::parameter) {
        args = false;
    } else {
        sel = dynamic_cast<ActualParameters *>(selectors[0].get());
        parameters(type, sel, values, proc->isExtern());
        args = true;
    }
    if (proc->isPredefined()) {
        vector<unique_ptr<ExpressionNode>> params;
        const auto callee = dynamic_cast<PredefinedProcedure *>(proc);
        value_ = createPredefinedCall(callee, ident, args ? sel->parameters() : params, values);
    } else {
        if (const auto fun = module_->getFunction(qualifiedName(proc))) {
            value_ = builder_.CreateCall(fun, values);
        } else {
            logger_.error(ident->start(), "undefined procedure: " + to_string(*ident) + ".");
        }
    }
    return this->selectors(&node, proc->getType()->getReturnType(), selectors.begin() + (args ? 1 : 0), selectors.end());
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
    const string val = node.value();
    const auto len = val.size() + 1;
    const auto type = ArrayType::get(builder_.getInt8Ty(), len);
    value_ = strings_[val];
    if (!value_) {
        const auto init = ConstantDataArray::getRaw(val, len, builder_.getInt8Ty());
        const auto str = new GlobalVariable(*module_, type, true, GlobalValue::PrivateLinkage, init, ".str");
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
    value_ = ConstantPointerNull::get(dyn_cast<PointerType>(getLLVMType(node.getCast())));
}

void LLVMIRBuilder::visit(SetLiteralNode &node) {
    value_ = ConstantInt::get(builder_.getInt32Ty(), static_cast<uint32_t>(node.value().to_ulong()));
}

void LLVMIRBuilder::visit(RangeLiteralNode &node) {
    value_ = ConstantInt::get(builder_.getInt32Ty(), static_cast<uint32_t>(node.value().to_ulong()));
}

void LLVMIRBuilder::installTrap(Value *cond, const uint8_t code) {
    const auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
    const auto trap = BasicBlock::Create(builder_.getContext(), "trap", function_);
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
    const auto cond = builder_.CreateICmpUGE(lsize, rsize);
    installTrap(cond, static_cast<uint8_t>(Trap::COPY_OVERFLOW));
}

void LLVMIRBuilder::trapNILPtr(Value *value) {
    const auto cond = builder_.CreateIsNotNull(value);
    installTrap(cond, static_cast<uint8_t>(Trap::NIL_POINTER));
}

void LLVMIRBuilder::trapIntDivByZero(Value *divisor) {
    const auto type = dyn_cast<IntegerType>(divisor->getType());
    const auto cond = builder_.CreateICmpNE(divisor, ConstantInt::get(type, 0));
    installTrap(cond, static_cast<uint8_t>(Trap::INT_DIVISION));
}

void LLVMIRBuilder::trapAssert(Value *cond) {
    installTrap(cond, static_cast<uint8_t>(Trap::ASSERT));
}

Value *LLVMIRBuilder::trapIntOverflow(Intrinsic::IndependentIntrinsics intrinsic, Value *lhs, Value *rhs) {
    const auto type = dyn_cast<IntegerType>(rhs->getType());
#ifndef _LLVM_20
    Function* fun = Intrinsic::getDeclaration(module_, intrinsic, { type });
#else
    Function* fun = Intrinsic::getOrInsertDeclaration(module_, intrinsic, { type });
#endif
    const auto call = builder_.CreateCall(fun, {lhs, rhs});
    const auto result = builder_.CreateExtractValue(call, {0});
    const auto status = builder_.CreateExtractValue(call, {1});
    const auto cond = builder_.CreateXor(status, builder_.getTrue());
    installTrap(cond, static_cast<uint8_t>(Trap::INT_OVERFLOW));
    return result;
}

void LLVMIRBuilder::trapFltDivByZero(Value *divisor) {
    const auto cond = builder_.CreateFCmpUNE(divisor, ConstantFP::get(divisor->getType(), 0));
    installTrap(cond, static_cast<uint8_t>(Trap::FLT_DIVISION));
}

void LLVMIRBuilder::trapSignConversion(Value *value) {
    Value *cond = builder_.CreateICmpSGE(value, ConstantInt::get(value->getType(), 0));
    installTrap(cond, static_cast<uint8_t>(Trap::SIGN_CONVERSION));
}

void LLVMIRBuilder::checkSignConversion(ExpressionNode &expr, Value *value) {
    if (config_.isSanitized(Trap::SIGN_CONVERSION)) {
        trapSignConversion(value);
    } else if (expr.isLiteral()) {
        // TODO This case should be handled by Sema.
        if (const auto val = dynamic_cast<IntegerLiteralNode *>(&expr)->value(); val < 0) {
            logger_.warning(expr.pos(), "implicit sign conversion can cause undefined behavior.");
        }
    }
}

Value *LLVMIRBuilder::createTypeTest(Value *td, TypeNode *type) {
    auto *record_t = dynamic_cast<RecordTypeNode *>(type);
    if (!record_t) {
        const auto pointer_t = dynamic_cast<PointerTypeNode *>(type);
        record_t = dynamic_cast<RecordTypeNode *>(pointer_t->getBase());
    }
    Value *level = builder_.getInt32(record_t->getLevel());
    Value *value = builder_.CreateLoad(builder_.getPtrTy(), td);
    auto len = builder_.CreateInBoundsGEP(recordTdTy_, value, {builder_.getInt32(0), builder_.getInt32(1)});
    len = builder_.CreateLoad(builder_.getInt32Ty(), len);
    auto cond = builder_.CreateICmpULE(level, len);
    const auto cur = builder_.GetInsertBlock();
    const auto test = BasicBlock::Create(builder_.getContext(), "test", function_);
    const auto skip = BasicBlock::Create(builder_.getContext(), "skip", function_);
    builder_.CreateCondBr(cond, test, skip);
    builder_.SetInsertPoint(test);
    const auto tds = builder_.CreateInBoundsGEP(recordTdTy_, value, {builder_.getInt32(0), builder_.getInt32(0)});
    value = builder_.CreateLoad(builder_.getPtrTy(), tds);
    const auto rid = builder_.CreateInBoundsGEP(builder_.getPtrTy(), value, {level});
    value = builder_.CreateLoad(builder_.getPtrTy(), rid);
    cond = builder_.CreateICmpEQ(recTypeIds_[record_t], value);
    builder_.CreateBr(skip);
    builder_.SetInsertPoint(skip);
    const auto phi = builder_.CreatePHI(builder_.getInt1Ty(), 2);
    phi->addIncoming(builder_.getFalse(), cur);
    phi->addIncoming(cond, test);
    return phi;
}

Value *LLVMIRBuilder::createTypeTest(Value *value, const NodeReference *ref, const TypeNode *lType, TypeNode *rType) {
    const auto prv = builder_.GetInsertBlock();
    BasicBlock *skip = nullptr;
    if (rType->isPointer()) {
        BasicBlock *test = BasicBlock::Create(builder_.getContext(), "test", function_);
        skip = BasicBlock::Create(builder_.getContext(), "skip", function_);
        builder_.CreateCondBr(builder_.CreateIsNotNull(value), test, skip);
        builder_.SetInsertPoint(test);
    }
    Value *res;
    if (lType->extends(rType)) {
        res = builder_.getTrue();
    } else {
        const auto td = getTypeDescriptor(value, ref, rType);
        res = createTypeTest(td, rType);
    }
    if (skip) {
        const auto cur= builder_.GetInsertBlock();
        builder_.CreateBr(skip);
        builder_.SetInsertPoint(skip);
        const auto phi = builder_.CreatePHI(builder_.getInt1Ty(), 2);
        phi->addIncoming(res, cur);
        phi->addIncoming(builder_.getFalse(), prv);
        return phi;
    }
    return res;
}

void LLVMIRBuilder::createNumericTestCase(const CaseOfNode &node, BasicBlock *dflt, BasicBlock *tail) {
    auto *type = dyn_cast<IntegerType>(getLLVMType(node.getExpression()->getType()));
    SwitchInst *inst = builder_.CreateSwitch(value_, dflt, static_cast<unsigned int>(node.getLabelCount()));
    for (size_t i = 0; i < node.getCaseCount(); ++i) {
        const auto block = BasicBlock::Create(builder_.getContext(), "case." + to_string(i), function_);
        const auto c = node.getCase(i);
        builder_.SetInsertPoint(block);
        c->getStatements()->accept(*this);
        ensureTerminator(tail);
        for (const int64_t label : c->getLabel()->getValues()) {
            ConstantInt* value = node.getExpression()->getType()->isChar() ?
                                 ConstantInt::get(type, static_cast<uint64_t>(label)) : ConstantInt::getSigned(type, label);
            inst->addCase(value, block);
        }
    }
}

void LLVMIRBuilder::createTypeTestCase(const CaseOfNode &node, BasicBlock *dflt, BasicBlock *tail) {
    const auto lhs = value_;
    const auto lExpr = dynamic_cast<QualifiedExpression *>(node.getExpression());
    const auto lType = lExpr->dereference()->getType();
    for (size_t i = 0; i < node.getLabelCount(); ++i) {
        const auto c = node.getCase(i);
        const auto label = c->getLabel()->getValue(0);
        const auto rExpr = dynamic_cast<QualifiedExpression *>(label);
        const auto rType = rExpr->dereference()->getType();
        auto name = rType->getIdentifier()->name();
        const auto is_true = BasicBlock::Create(builder_.getContext(), "is" + name + "_true", function_);
        const auto is_false = BasicBlock::Create(builder_.getContext(), "is" + name + "_false", function_);
        const auto cond = createTypeTest(lhs, lExpr, lType, rType);
        builder_.CreateCondBr(cond, is_true, is_false);
        builder_.SetInsertPoint(is_true);
        lExpr->dereference()->setType(rType);   // Set the formal type to the actual type
        c->getStatements()->accept(*this);
        ensureTerminator(tail);
        lExpr->dereference()->setType(lType);   // Reset the formal type of the case expression
        builder_.SetInsertPoint(is_false);
    }
    builder_.CreateBr(dflt);
}

Value *LLVMIRBuilder::createStringComparison(const BinaryExpressionNode *node) {
    if (node->getOperator() == OperatorType::EQ || node->getOperator() == OperatorType::NEQ) {
        const auto lhs = node->getLeftExpression();
        const auto rhs = node->getRightExpression();
        const auto lhsType = lhs->getType();
        const auto rhsType = rhs->getType();
        setRefMode(false);
        lhs->accept(*this);
        restoreRefMode();
        auto lhsValue = value_;
        setRefMode(false);
        rhs->accept(*this);
        restoreRefMode();
        auto rhsValue = value_;
        Value *value = nullptr;
        Value *test = builder_.getInt32(0);
        if ((lhsType->isArray() && rhsType->isArray()) || lhsType->isString() || rhsType->isString()) {
            // TODO This introduces a dependency to `strcmp` that might not be ideal.
            auto fun = module_->getFunction("strcmp");
            if (!fun) {
                const auto type = FunctionType::get(builder_.getInt32Ty(), {builder_.getPtrTy(), builder_.getPtrTy()}, false);
                fun = Function::Create(type, GlobalValue::ExternalLinkage, "strcmp", module_);
                // fun->addFnAttr(Attribute::getWithAllocSizeArgs(builder_.getContext(), 0, {}));
                fun->addParamAttr(0, Attribute::NoUndef);
                fun->addParamAttr(1, Attribute::NoUndef);
            }
            value = builder_.CreateCall(FunctionCallee(fun), {lhsValue, rhsValue});
        } else {
            value = lhsType->isChar() ? rhsValue : lhsValue;
            value = builder_.CreateInBoundsGEP(ArrayType::get(builder_.getInt8Ty(), 0), value,
                                               {builder_.getInt32(0), builder_.getInt32(0)});
            value = builder_.CreateLoad(builder_.getInt8Ty(), value);
            test = lhsType->isChar() ? lhsValue : rhsValue;
        }
        if (node->getOperator() == OperatorType::EQ) {
            value = builder_.CreateICmpEQ(value, test);
        } else {
            value = builder_.CreateICmpNE(value, test);
        }
        return value;
    }
    logger_.error(node->pos(), "operator " + to_string(node->getOperator()) + " not applicable to strings.");
    return value_;
}

Value *LLVMIRBuilder::createNeg(Value *value) {
    if (config_.isSanitized(Trap::INT_OVERFLOW)) {
        const auto type = dyn_cast<IntegerType>(value->getType());
        return trapIntOverflow(Intrinsic::ssub_with_overflow, ConstantInt::get(type, 0), value);
    }
    return builder_.CreateNeg(value);
}

Value *LLVMIRBuilder::createAdd(Value *lhs, Value *rhs) {
    const bool sanitize = config_.isSanitized(Trap::INT_OVERFLOW);
    return sanitize ? trapIntOverflow(Intrinsic::sadd_with_overflow, lhs, rhs) : builder_.CreateAdd(lhs, rhs);
}

Value *LLVMIRBuilder::createSub(Value *lhs, Value *rhs) {
    const bool sanitize = config_.isSanitized(Trap::INT_OVERFLOW);
    return sanitize ? trapIntOverflow(Intrinsic::ssub_with_overflow, lhs, rhs) : builder_.CreateSub(lhs, rhs);
}

Value *LLVMIRBuilder::createMul(Value *lhs, Value *rhs) {
    const bool sanitize = config_.isSanitized(Trap::INT_OVERFLOW);
    return sanitize ? trapIntOverflow(Intrinsic::smul_with_overflow, lhs, rhs) : builder_.CreateMul(lhs, rhs);
}

Value *LLVMIRBuilder::createDiv(Value *lhs, Value *rhs) {
    if (config_.isSanitized(Trap::INT_DIVISION)) {
        trapIntDivByZero(rhs);
    }
    if (config_.isSanitized(Trap::SIGN_CONVERSION)) {
        trapSignConversion(rhs);
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
    if (config_.isSanitized(Trap::SIGN_CONVERSION)) {
        trapSignConversion(rhs);
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
    const auto type = node.getType();
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
    const auto lhsType = node.getLeftExpression()->getType();
    const auto rhsType = node.getRightExpression()->getType();
    if ((lhsType->isArray() && rhsType->isArray()) ||
        (lhsType->isArray() && (rhsType->isString() || rhsType->isChar())) ||
        (rhsType->isArray() && (lhsType->isString() || lhsType->isChar()))) {
        value_ = createStringComparison(&node);
        return;
    }
    node.getLeftExpression()->accept(*this);
    cast(*node.getLeftExpression());
    const auto lhs = value_;
    // Logical operators `&` and `OR` are treated explicitly to enable short-circuiting.
    if (node.getOperator() == OperatorType::AND || node.getOperator() == OperatorType::OR) {
        // Create one block to evaluate the right-hand side and one to skip it.
        const auto eval = BasicBlock::Create(builder_.getContext(), "eval", function_);
        const auto skip = BasicBlock::Create(builder_.getContext(), "skip", function_);
        // Insert branch to decide whether to skip `&` or `OR`
        if (node.getOperator() == OperatorType::AND) {
            builder_.CreateCondBr(value_, eval, skip);
        } else if (node.getOperator() == OperatorType::OR) {
            builder_.CreateCondBr(value_, skip, eval);
        }
        // Save the current (left-hand side) block.
        const auto lhsBB = builder_.GetInsertBlock();
        // Create and populate the basic block to evaluate the right-hand side, if required.
        builder_.SetInsertPoint(eval);
        node.getRightExpression()->accept(*this);
        const auto rhs = value_;
        if (node.getOperator() == OperatorType::AND) {
            value_ = builder_.CreateAnd(lhs, rhs);
        } else if (node.getOperator() == OperatorType::OR) {
            value_ = builder_.CreateOr(lhs, rhs);
        }
        // Save the current (right-hand side) block.
        const auto rhsBB = builder_.GetInsertBlock();
        builder_.CreateBr(skip);
        // Create the basic block after the possible short-circuit,
        // use phi-value to "combine" value of both branches.
        builder_.SetInsertPoint(skip);
        const auto phi = builder_.CreatePHI(value_->getType(), 2, "phi");
        phi->addIncoming(lhs, lhsBB);
        phi->addIncoming(value_, rhsBB);
        value_ = phi;
    } else if (node.getOperator() == OperatorType::IS) {
        const auto lExpr = dynamic_cast<QualifiedExpression *>(node.getLeftExpression());
        const auto lType = lExpr->getType();
        const auto rExpr = dynamic_cast<QualifiedExpression *>(node.getRightExpression());
        const auto rType = rExpr->dereference()->getType();
        value_ = createTypeTest(lhs, lExpr, lType, rType);
    } else if (lhsType->isSet() || rhsType->isSet()) {
        node.getRightExpression()->accept(*this);
        cast(*node.getRightExpression());
        const auto rhs = value_;
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
    } else if (lhsType->isNumeric() && rhsType->isNumeric()) {
        node.getRightExpression()->accept(*this);
        cast(*node.getRightExpression());
        const auto rhs = value_;
        const bool floating = lhsType->isReal() || rhsType->isReal();
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
            case OperatorType::DIV:
                value_ = createDiv(lhs, rhs);
                break;
            case OperatorType::MOD:
                value_ = createMod(lhs, rhs);
                break;
            case OperatorType::EQ:
                value_ = floating ? builder_.CreateFCmpUEQ(lhs, rhs) : builder_.CreateICmpEQ(lhs, rhs);
                break;
            case OperatorType::NEQ:
                value_ = floating ? builder_.CreateFCmpUNE(lhs, rhs) : builder_.CreateICmpNE(lhs, rhs);
                break;
            case OperatorType::LT:
                value_ = floating ? builder_.CreateFCmpULT(lhs, rhs) : builder_.CreateICmpSLT(lhs, rhs);
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
    } else if (lhsType->isChar() && rhsType->isChar()) {
        node.getRightExpression()->accept(*this);
        cast(*node.getRightExpression());
        const auto rhs = value_;
        switch (node.getOperator()) {
            case OperatorType::EQ:
                value_ = builder_.CreateICmpEQ(lhs, rhs); break;
            case OperatorType::NEQ:
                value_ = builder_.CreateICmpNE(lhs, rhs); break;
            case OperatorType::LT:
                value_ = builder_.CreateICmpULT(lhs, rhs); break;
            case OperatorType::GT:
                value_ = builder_.CreateICmpUGT(lhs, rhs); break;
            case OperatorType::LEQ:
                value_ = builder_.CreateICmpULE(lhs, rhs); break;
            case OperatorType::GEQ:
                value_ = builder_.CreateICmpUGE(lhs, rhs); break;
            default:
                value_ = nullptr;
                logger_.error(node.pos(), "operator " + to_string(node.getOperator()) + " not applicable here.");
                break;
        }
    } else if (lhsType->isPointer() || rhsType->isPointer()) {
        node.getRightExpression()->accept(*this);
        cast(*node.getRightExpression());
        const auto rhs = value_;
        if (node.getOperator() == OperatorType::EQ) {
            value_ = builder_.CreateICmpEQ(lhs, rhs);
        } else if (node.getOperator() == OperatorType::NEQ) {
            value_ = builder_.CreateICmpNE(lhs, rhs);
        } else {
            logger_.error(node.pos(), "operator " + to_string(node.getOperator()) + " not applicable to pointers.");
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
    const string name = node.getIdentifier()->name();
    scopes_.push_back(name);
    getLLVMType(node.getType());
    scopes_.pop_back();
}

void LLVMIRBuilder::visit(ArrayTypeNode &node) {
    types_[&node] = ArrayType::get(getLLVMType(node.types()[0]), node.lengths()[0]);
    // If necessary, create dope vector
    if (node.getBase() == &node && !node.isOpen()) {
        vector<Constant *> dims;
        for (const auto len: node.lengths()) {
            dims.push_back(ConstantInt::get(builder_.getInt64Ty(), len));
        }
        const auto dvType = ArrayType::get(builder_.getInt64Ty(), node.dimensions());
        const auto init = ConstantArray::get(dvType, dims);
        const string id = node.getIdentifier() ? node.getIdentifier()->name() : "";
        const auto dv = new GlobalVariable(*module_, dvType, true, GlobalValue::InternalLinkage, init, id + ".dv");
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
            logger_.error(node.pos(), "cannot map type " + to_string(node.kind()) + " to LLVM intermediate representation.");
            types_[&node] = builder_.getVoidTy();
    }
}

void LLVMIRBuilder::visit(ProcedureTypeNode &node) {
    vector<Type*> params;
    for (const auto &param : node.parameters()) {
        const auto type = param->getType();
        auto param_t = getLLVMType(type);
        params.push_back(param->isVar() || type->isStructured() ? PointerType::get(param_t, 0) : param_t);
        // Add a parameter for the "dope vector" in case of an open array parameter
        // or for the type descriptor in case of a variable record parameter
        if ((type->isArray() && dynamic_cast<ArrayTypeNode*>(type)->isOpen()) || (type->isRecord() && param->isVar())) {
            params.push_back(builder_.getPtrTy());
        }
    }
    const auto type = FunctionType::get(getLLVMType(node.getReturnType()), params, node.hasVarArgs());
    funTypes_[&node] = type;
    types_[&node] = builder_.getPtrTy();
}

void LLVMIRBuilder::visit(RecordTypeNode &node) {
    // Create an empty struct and add it to the lookup table immediately to support recursive records
    const auto structTy = StructType::create(builder_.getContext());
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
        const auto fieldTy = node.getField(i)->getType();
        elemTys.push_back(getLLVMType(fieldTy));
    }
    structTy->setBody(elemTys);
    if (node.getModule() == ast_->getTranslationUnit()) {
        name = module_->getModuleIdentifier() + "__" + name;
        // Create an id for the record type by defining a global int32 variable and using its address
        const auto id = new GlobalVariable(*module_, builder_.getInt32Ty(), true, GlobalValue::ExternalLinkage,
                                     Constant::getNullValue(builder_.getInt32Ty()), name + "_id");
        id->setAlignment(module_->getDataLayout().getPreferredAlign(id));
        recTypeIds_[&node] = id;
        // Collect the ids of the record types from which this type is extended
        vector<Constant *> typeIds;
        auto cur = &node;
        auto *nil = dyn_cast<Constant>(ConstantPointerNull::get(builder_.getPtrTy()));
        while (cur->isExtended()) {
            typeIds.insert(typeIds.begin(), recTypeIds_[cur] ? recTypeIds_[cur] : nil);
            cur = cur->getBaseType();
        }
        typeIds.insert(typeIds.begin(), recTypeIds_[cur] ? recTypeIds_[cur] : nil);
        const auto idsType = ArrayType::get(builder_.getPtrTy(), typeIds.size());
        auto ids = new GlobalVariable(*module_, idsType, true, GlobalValue::ExternalLinkage,
                                      ConstantArray::get(idsType, typeIds), name + "_ids");
        ids->setAlignment(module_->getDataLayout().getPreferredAlign(ids));
        // Create the type descriptor
        const auto td = new GlobalVariable(*module_, recordTdTy_, true, GlobalValue::ExternalLinkage,
                                     ConstantStruct::get(recordTdTy_, {ids, ConstantInt::get(builder_.getInt32Ty(),
                                                                                             node.getLevel())}),
                                     name + "_td");
        td->setAlignment(module_->getDataLayout().getPreferredAlign(td));
        recTypeTds_[&node] = td;
        if (triple_.isOSWindows() && !node.isAnonymous() && node.getIdentifier()->isExported() && !config_.hasFlag(Flag::ENABLE_MAIN)) {
            id->setDLLStorageClass(GlobalValue::DLLStorageClassTypes::DLLExportStorageClass);
            td->setDLLStorageClass(GlobalValue::DLLStorageClassTypes::DLLExportStorageClass);
        }
    } else if (!node.isAnonymous()){
        name = node.getModule()->getIdentifier()->name() + "__" + name;
        // Create an external constant to be used as the id of the imported record type
        const auto id = new GlobalVariable(*module_, builder_.getInt32Ty(), true, GlobalValue::ExternalLinkage, nullptr,
                                     name + "_id");
        id->setAlignment(module_->getDataLayout().getPreferredAlign(id));
        recTypeIds_[&node] = id;
        // Create an external constant to be used as the type descriptor of the imported record type
        const auto td = new GlobalVariable(*module_, recordTdTy_, true, GlobalValue::ExternalLinkage, nullptr,
                                     name + "_td");
        td->setAlignment(module_->getDataLayout().getPreferredAlign(td));
        recTypeTds_[&node] = td;
    }
}

void LLVMIRBuilder::visit(PointerTypeNode &node) {
    const auto base = node.getBase();
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
    const auto rType = node.getRvalue()->getType();
    const auto lType = node.getLvalue()->getType();
    setRefMode(!rType->isStructured() && !rType->isString());
    node.getRvalue()->accept(*this);
    cast(*node.getRvalue());
    Value* rValue = value_;
    restoreRefMode();
    node.getLvalue()->accept(*this);
    Value* lValue = value_;
    if (rType->isArray()) {
        const auto lArray = dynamic_cast<ArrayTypeNode *>(lType);
        const auto rArray = dynamic_cast<ArrayTypeNode *>(rType);
        const auto lLen = lArray->isOpen() ? 0 : lArray->lengths()[0];
        const auto rLen = rArray->isOpen() ? 0 : rArray->lengths()[0];
        const auto len = std::min(lLen, rLen);
        const auto layout = module_->getDataLayout();
        const auto elemSize = layout.getTypeAllocSize(getLLVMType(lArray->getMemberType()));
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
        const auto layout = module_->getDataLayout();
        const auto size = layout.getTypeAllocSize(getLLVMType(lType));
        value_ = builder_.CreateMemCpy(lValue, {}, rValue, {}, builder_.getInt64(size));
    } else if (rType->isString()) {
        const auto str = dynamic_cast<StringLiteralNode *>(node.getRvalue());
        const auto value = str->value();
        Value *len = builder_.getInt64(value[0] == '\0' ? 1 : value.size() + 1);
        value_ = builder_.CreateMemCpy(lValue, {}, rValue, {}, len);
    } else {
        value_ = builder_.CreateStore(rValue, lValue);
    }
}

void LLVMIRBuilder::visit(CaseOfNode& node) {
    const auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
    auto dflt = tail;
    if (node.hasElse()) {
        dflt = BasicBlock::Create(builder_.getContext(), "default", function_);
    }
    setRefMode(true);
    node.getExpression()->accept(*this);
    restoreRefMode();
    // Check whether the case statement is a numeric or a type test
    const auto type = node.getExpression()->getType();
    if (type->isInteger() || type->isChar()) {
        createNumericTestCase(node, dflt, tail);
    } else {
        createTypeTestCase(node, dflt, tail);
    }
    if (node.hasElse()) {
        builder_.SetInsertPoint(dflt);
        node.getElseStatements()->accept(*this);
        ensureTerminator(tail);
    }
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(CaseLabelNode&) {}

void LLVMIRBuilder::visit(CaseNode&) {}

void LLVMIRBuilder::visit(IfThenElseNode &node) {
    const auto tail = BasicBlock::Create(builder_.getContext(), "if_tail", function_);
    const auto if_true = BasicBlock::Create(builder_.getContext(), "if_true", function_);
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
    ensureTerminator(tail);
    builder_.SetInsertPoint(if_false);
    for (size_t i = 0; i < node.getElseIfCount(); i++) {
        const auto elsif = node.getElseIf(i);
        const auto elsif_true = BasicBlock::Create(builder_.getContext(), "elsif_true", function_);
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
        ensureTerminator(tail);
        builder_.SetInsertPoint(elsif_false);
    }
    if (node.hasElse()) {
        node.getElseStatements()->accept(*this);
        ensureTerminator(tail);
    }
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(ElseIfNode &) {}

void LLVMIRBuilder::visit(LoopNode &node) {
    const auto body = BasicBlock::Create(builder_.getContext(), "loop_body", function_);
    const auto tail = BasicBlock::Create(builder_.getContext(), "loop_tail", function_);
    builder_.CreateBr(body);
    // Loop body
    builder_.SetInsertPoint(body);
    loopTails_.push(tail);
    node.getStatements()->accept(*this);
    loopTails_.pop();
    // Unconditional branch back to loop body
    ensureTerminator(body);
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(WhileLoopNode &node) {
    const auto tail = BasicBlock::Create(builder_.getContext(), "while_tail", function_);
    const auto body = BasicBlock::Create(builder_.getContext(), "while_body", function_);
    const auto while_true = BasicBlock::Create(builder_.getContext(), "while_true", function_);
    auto while_false = tail;
    if (node.hasElseIf()) {
        while_false = BasicBlock::Create(builder_.getContext(), "while_false", function_);
    }
    builder_.CreateBr(body);
    builder_.SetInsertPoint(body);
    // Check whether while loop is skipped
    setRefMode(true);
    node.getCondition()->accept(*this);
    restoreRefMode();
    builder_.CreateCondBr(value_, while_true, while_false);
    // While loop body
    builder_.SetInsertPoint(while_true);
    loopTails_.push(tail);
    node.getStatements()->accept(*this);
    loopTails_.pop();
    // Unconditional branch back to loop body
    ensureTerminator(body);
    builder_.SetInsertPoint(while_false);
    for (size_t i = 0; i < node.getElseIfCount(); i++) {
        const auto elsif = node.getElseIf(i);
        const auto elsif_true = BasicBlock::Create(builder_.getContext(), "elsif_true", function_);
        auto elsif_false = tail;
        if (i + 1 < node.getElseIfCount()) {
            elsif_false = BasicBlock::Create(builder_.getContext(), "elsif_false", function_);
        }
        setRefMode(true);
        elsif->getCondition()->accept(*this);
        restoreRefMode();
        builder_.CreateCondBr(value_, elsif_true, elsif_false);
        builder_.SetInsertPoint(elsif_true);
        elsif->getStatements()->accept(*this);
        ensureTerminator(body);
        builder_.SetInsertPoint(elsif_false);
    }
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(RepeatLoopNode &node) {
    const auto body = BasicBlock::Create(builder_.getContext(), "repeat_body", function_);
    const auto tail = BasicBlock::Create(builder_.getContext(), "repeat_tail", function_);
    builder_.CreateBr(body);
    // Repeat loop body
    builder_.SetInsertPoint(body);
    loopTails_.push(tail);
    node.getStatements()->accept(*this);
    loopTails_.pop();
    // Check whether repeat loop is continued
    if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
        setRefMode(true);
        node.getCondition()->accept(*this);
        restoreRefMode();
        builder_.CreateCondBr(value_, tail, body);
    }
    builder_.SetInsertPoint(tail);
}

void LLVMIRBuilder::visit(ForLoopNode &node) {
    // Initialize loop counter
    setRefMode(true);
    node.getLow()->accept(*this);
    const auto start = value_;
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
    const auto body = BasicBlock::Create(builder_.getContext(), "for_body", function_);
    const auto tail = BasicBlock::Create(builder_.getContext(), "for_tail", function_);
    const auto step = dynamic_cast<IntegerLiteralNode*>(node.getStep())->value();
    if (step > 0) {
        value_ = builder_.CreateICmpSLE(counter, end);
    } else {
        value_ = builder_.CreateICmpSGE(counter, end);
    }
    builder_.CreateCondBr(value_, body, tail);
    // Loop body
    builder_.SetInsertPoint(body);
    loopTails_.push(tail);
    node.getStatements()->accept(*this);
    loopTails_.pop();
    // Update loop counter
    if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
        setRefMode(true);
        node.getCounter()->accept(*this);
        restoreRefMode();
        counter = builder_.CreateAdd(value_, ConstantInt::getSigned(builder_.getInt32Ty(), step));
        node.getCounter()->accept(*this);
        const auto lValue = value_;
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
    }
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

void LLVMIRBuilder::visit(ExitNode &) {
    builder_.CreateBr(loopTails_.top());
}

void LLVMIRBuilder::cast(const ExpressionNode &node) {
    const auto target = node.getCast();
    const auto source = node.getType();
    if (target && target != source) {
        if (source->isInteger() || source->isByte()) {
            if (target->isInteger() || target->isByte()) {
                if (target->getSize() > source->getSize()) {
                    if (source->isByte()) {
                        value_ = builder_.CreateZExt(value_, getLLVMType(target));
                    } else {
                        value_ = builder_.CreateSExt(value_, getLLVMType(target));
                    }
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
            const auto type = dynamic_cast<ArrayTypeNode *>(source);
            if (type->getMemberType()->kind() != TypeKind::CHAR) {
                logger_.error(node.pos(), "cannot cast " + to_string(*source->getIdentifier()) + " to " +
                                           to_string(*target->getIdentifier()) + ".");
            }
        }
    }
}

Value *
LLVMIRBuilder::createPredefinedCall(const PredefinedProcedure *proc, const QualIdent *ident,
                                    const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    switch (const ProcKind kind = proc->getKind()) {
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
        const auto funTy = FunctionType::get(builder_.getVoidTy(), {}, false);
        fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "abort", module_);
        fun->addFnAttr(Attribute::Cold);
        fun->addFnAttr(Attribute::NoReturn);
    }
    builder_.CreateCall(FunctionCallee(fun), {});
    return builder_.CreateUnreachable();
}

Value *
LLVMIRBuilder::createAbsCall(const TypeNode *type, Value *param) {
    if (type->isInteger()) {
        value_ = builder_.CreateIntrinsic(Intrinsic::abs, {param->getType()}, {param, builder_.getInt1(true)});
    } else {
        value_ = builder_.CreateIntrinsic(Intrinsic::fabs, {param->getType()}, {param});
    }
    return value_;
}

Value *
LLVMIRBuilder::createAsrCall(const vector<unique_ptr<ExpressionNode>> &actuals, const std::vector<Value *> &params) {
    Value *shift = params[1];
    checkSignConversion(*actuals[1], shift);
    if (params[0]->getType()->getIntegerBitWidth() > shift->getType()->getIntegerBitWidth()) {
        shift = builder_.CreateZExt(shift, params[0]->getType());
    } else if (params[0]->getType()->getIntegerBitWidth() < shift->getType()->getIntegerBitWidth()) {
        shift = builder_.CreateTrunc(shift, params[0]->getType());
    }
    return builder_.CreateAShr(params[0], shift);
}

Value *
LLVMIRBuilder::createAssertCall(Value *param) {
    if (config_.isSanitized(Trap::ASSERT)) {
        trapAssert(param);
    } else {
        const auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
        const auto abort = BasicBlock::Create(builder_.getContext(), "abort", function_);
        builder_.CreateCondBr(param, tail, abort);
        builder_.SetInsertPoint(abort);
        value_ = createAbortCall();
        builder_.SetInsertPoint(tail);
    }
    return value_;
}

Value *
LLVMIRBuilder::createChrCall(Value *param) {
    value_ = builder_.CreateTrunc(param, builder_.getInt8Ty());
    return value_;
}

Value *
LLVMIRBuilder::createEntireCall(Value *param) {
    Value *value = builder_.CreateIntrinsic(Intrinsic::floor, {param->getType()}, {param});
    value_ = builder_.CreateFPToSI(value, builder_.getInt64Ty());
    return value_;
}

Value *
LLVMIRBuilder::createFltCall(Value *param) {
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
LLVMIRBuilder::createUnpkCall(const vector<unique_ptr<ExpressionNode>> &actuals, const std::vector<Value *> &params) {
    const auto xtype = getLLVMType(actuals[0]->getType());
    Value *xval = builder_.CreateLoad(xtype, params[0]);
    if (xtype->isFloatTy()) {
        xval = builder_.CreateFPExt(xval, builder_.getDoubleTy());
    }
    const auto ret = builder_.CreateIntrinsic(Intrinsic::frexp, {builder_.getDoubleTy(), builder_.getInt32Ty()}, xval); // Usually only Double supported by targets
    const auto y = ConstantFP::get(builder_.getDoubleTy(), 2.0);
    xval = builder_.CreateFMul(y, builder_.CreateExtractValue(ret, {0}));
    if (xtype->isFloatTy()) {
        xval = builder_.CreateFPTrunc(xval, builder_.getFloatTy());  
    }
    builder_.CreateStore(xval, params[0]);
    const auto ntype = getLLVMType(actuals[1]->getType());
    const auto z = ConstantInt::get(ntype, 1);
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
        const auto funTy = FunctionType::get(builder_.getVoidTy(), { builder_.getInt32Ty() }, false);
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
LLVMIRBuilder::createIncDecCall(const ProcKind kind,
                                const vector<unique_ptr<ExpressionNode>> &actuals, const std::vector<Value *> &params) {
    const auto target = getLLVMType(actuals[0]->getType());
    Value *delta;
    if (params.size() > 2) {
        auto param = actuals[2].get();
        logger_.error(param->pos(), "more actual than formal parameters.");
        return value_;
    }
    if (params.size() > 1) {
        const auto param0 = actuals[0].get();
        const auto param1 = actuals[1].get();
        if (!param1->getType()->isInteger()) {
            logger_.error(param1->pos(), "type mismatch: expected integer type, found " +
                                         to_string(param1->getType()) + ".");
            return value_;
        }
        const auto source = params[1]->getType();
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
LLVMIRBuilder::createInclCall(Value *set, Value *element) {
    Value *value = builder_.CreateShl(ConstantInt::get(builder_.getInt32Ty(), 0x1), element);
    value_ = builder_.CreateLoad(builder_.getInt32Ty(), set);
    value_ = builder_.CreateOr(value_, value);
    return builder_.CreateStore(value_, set);
}

Value *
LLVMIRBuilder::createLenCall(const vector<unique_ptr<ExpressionNode>> &actuals, const std::vector<Value *> &params) {
    // `params[0]`: the array for which the length will be returned
    // `params[1]`: pointer to the dope vector of the array
    // `params[2]`: the (optional) dimension of the array
    const auto param0 = actuals[0].get();
    if (param0->getType()->isString() && param0->isLiteral()) {
        const auto str = dynamic_cast<StringLiteralNode *>(param0);
        const string value = str->value();
        if (value[0] == '\0') {
            value_ = builder_.getInt64(0);
        } else {
            value_ = builder_.getInt64(value.size());
        }
        return value_;
    }
    const auto array_t = dynamic_cast<ArrayTypeNode *>(param0->getType());
    int64_t dim = 0;
    if (params.size() > 3) {
        const auto param = actuals[2].get();
        logger_.error(param->pos(), "more actual than formal parameters.");
        return value_;
    }
    if (params.size() > 2) {
        const auto param1 = actuals[1].get();
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
        value_ = builder_.getInt64(array_t->lengths()[static_cast<size_t>(dim)]);
        return value_;
    }
    return getOpenArrayLength(params[1], array_t, static_cast<uint32_t>(dim), false);
}

Value *
LLVMIRBuilder::createLongCall(const ExpressionNode *expr, Value *param) {
    if (const auto type = expr->getType(); type->isInteger()) {
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
LLVMIRBuilder::createLslCall(const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    Value *shift = params[1];
    checkSignConversion(*actuals[1], shift);
    if (params[0]->getType()->getIntegerBitWidth() > shift->getType()->getIntegerBitWidth()) {
        shift = builder_.CreateZExt(shift, params[0]->getType());
    } else if (params[0]->getType()->getIntegerBitWidth() < shift->getType()->getIntegerBitWidth()) {
        shift = builder_.CreateTrunc(shift, params[0]->getType());
    }
    return builder_.CreateShl(params[0], shift);
}

Value *
LLVMIRBuilder::createMaxMinCall(ExpressionNode *actual, const bool isMax) {
    const auto decl = dynamic_cast<QualifiedExpression *>(actual)->dereference();
    if (const auto type = dynamic_cast<TypeDeclarationNode *>(decl)->getType(); type->isReal()) {
        if (type->getSize() == 4) {
            value_ = ConstantFP::getInfinity(builder_.getFloatTy(), !isMax);
        } else {
            value_ = ConstantFP::getInfinity(builder_.getDoubleTy(), !isMax);
        }
    } else if (type->isInteger()) {
        if (type->getSize() == 8) {
            if (isMax) {
                value_ = builder_.getInt64(std::numeric_limits<int64_t>::max());
            } else {
                value_ = builder_.getInt64(static_cast<uint64_t>(std::numeric_limits<int64_t>::min()));
            }
        } else if (type->getSize() == 4) {
            if (isMax) {
                value_ = builder_.getInt32(std::numeric_limits<int32_t>::max());
            } else {
                value_ = builder_.getInt32(static_cast<uint32_t>(std::numeric_limits<int32_t>::min()));
            }
        } else {
            if (isMax) {
                value_ = builder_.getInt16(std::numeric_limits<int16_t>::max());
            } else {
                value_ = builder_.getInt16(static_cast<uint16_t>(std::numeric_limits<int16_t>::min()));
            }
        }
    } else {
        logger_.error(actual->pos(), "type mismatch: REAL or INTEGER expected.");
    }
    return value_;
}

Value *
LLVMIRBuilder::createNewCall(TypeNode *type, Value *param) {
    auto fun = module_->getFunction("malloc");
    if (!fun) {
        const auto funTy = FunctionType::get(builder_.getPtrTy(), { builder_.getInt64Ty() }, false);
        fun = Function::Create(funTy, GlobalValue::ExternalLinkage, "malloc", module_);
        fun->addFnAttr(Attribute::getWithAllocSizeArgs(builder_.getContext(), 0, {}));
        fun->addParamAttr(0, Attribute::NoUndef);
    }
    std::vector<Value *> values;
    const auto ptr = dynamic_cast<PointerTypeNode *>(type);
    const auto layout = module_->getDataLayout();
    const auto base = ptr->getBase();
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
LLVMIRBuilder::createOddCall(Value *param) {
    const auto paramTy = param->getType();
    value_ = builder_.CreateAnd(param, ConstantInt::get(paramTy, 1));
    return builder_.CreateICmpEQ(value_, ConstantInt::get(paramTy, 1));
}

Value *
LLVMIRBuilder::createOrdCall(const ExpressionNode *actual, Value *param) {
    if (actual->getType()->isBoolean()) {
        Value *value = builder_.CreateAlloca(builder_.getInt32Ty());
        const auto tail = BasicBlock::Create(builder_.getContext(), "tail", function_);
        const auto return1 = BasicBlock::Create(builder_.getContext(), "true", function_);
        const auto return0 = BasicBlock::Create(builder_.getContext(), "false", function_);
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
LLVMIRBuilder::createRorCall(const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    Value *shift = params[1];
    checkSignConversion(*actuals[1], shift);
    if (params[0]->getType()->getIntegerBitWidth() > shift->getType()->getIntegerBitWidth()) {
        shift = builder_.CreateZExt(shift, params[0]->getType());
    } else if (params[0]->getType()->getIntegerBitWidth() < shift->getType()->getIntegerBitWidth()) {
        shift = builder_.CreateTrunc(shift, params[0]->getType());
    }
    Value *lhs = builder_.CreateLShr(params[0], shift);
    Value *value = ConstantInt::get(params[0]->getType(), params[0]->getType()->getIntegerBitWidth());
    Value *delta = builder_.CreateSub(value, shift);
    Value *rhs = builder_.CreateShl(params[0], delta);
    return builder_.CreateOr(lhs, rhs);
}

Value *
LLVMIRBuilder::createShortCall(const ExpressionNode *expr, Value *param) {
    if (expr->isLiteral()) {
        logger_.error(expr->pos(), "constant not valid parameter.");
        return value_;
    }
    if (const auto type = expr->getType(); type->isInteger()) {
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
    const auto decl = dynamic_cast<QualifiedExpression *>(expr)->dereference();
    const auto type = dynamic_cast<TypeDeclarationNode *>(decl);
    const auto layout = module_->getDataLayout();
    const auto size = layout.getTypeAllocSize(getLLVMType(type->getType()));
    value_ = builder_.getInt64(size);
    return value_;
}

Value *
LLVMIRBuilder::createSystemAdrCall(const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    // TODO Handle procedure reference
    const auto actual = actuals[0].get();
    const auto type = actual->getType();
    // TODO Isn't CHAR a basic type?
    if (type->isChar() || type->isBasic()) {
        value_ = builder_.CreatePtrToInt(params[0], builder_.getInt64Ty());
    } else if (type->isArray() || type->isString()) {
        auto arrayTy = getLLVMType(type);
        vector<Value *> indices;
        indices.push_back(builder_.getInt32(0));
        if (type->isString() && actual->isLiteral()) {
            const auto str = dynamic_cast<StringLiteralNode *>(actual);
            const auto stringTy = ArrayType::get(builder_.getInt8Ty(), str->value().size() + 1);
            arrayTy = StructType::get(builder_.getInt64Ty(), stringTy);
            // indices.push_back(builder_.getInt32(1));
            // indices.push_back(builder_.getInt32(0));
        } else {
            const auto atype = dynamic_cast<ArrayTypeNode *>(type);
            // TODO Isn't CHAR a basic type?
            if (!atype->getMemberType()->isChar() && !atype->getMemberType()->isBasic()) {
                logger_.error(actual->pos(), "expected array of basic type or CHAR");
                return value_;
            }
        }
        value_ = builder_.CreateInBoundsGEP(arrayTy, params[0], indices);
    } else if (type->isRecord()) {
        const auto recordTy = getLLVMType(type);
        vector<Value *> indices;
        indices.push_back(builder_.getInt32(0));
        const auto rtype = dynamic_cast<RecordTypeNode *>(type);
        for (size_t i = 0; i < rtype->getFieldCount(); i++) {
            const auto fieldTy = rtype->getField(i)->getType();
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
LLVMIRBuilder::createSystemGetCall(const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    auto const param = actuals[1].get();
    auto const type = param->getType();
    if (type->isChar() || type->isBasic()) {
        const auto base = getLLVMType(type);
        const auto ptrtype = PointerType::get(base, 0);
        const auto ptr = builder_.CreateIntToPtr(params[0], ptrtype);
        const auto value = builder_.CreateLoad(base, ptr, true);
        value_ = builder_.CreateStore(value, params[1]);
    } else {
        logger_.error(param->pos(), "expected basic or char type");
    }
    return value_;
}

Value *
LLVMIRBuilder::createSystemPutCall(const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    const auto param = actuals[1].get();
    const auto type = param->getType();
    if (type->isChar() || type->isBasic()) {
        const auto base = getLLVMType(type);
        const auto ptrtype = PointerType::get(base, 0);
        const auto ptr = builder_.CreateIntToPtr(params[0], ptrtype);
        value_ = builder_.CreateStore(params[1], ptr, true);
    } else {
        logger_.error(param->pos(), "expected basic or char type");
    }
    return value_;
}

Value *
LLVMIRBuilder::createSystemBitCall(const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    const auto param1 = actuals[1].get();
    if (param1->isLiteral()) {
        const auto val = dynamic_cast<IntegerLiteralNode *>(param1)->value();
        if (val < 0 || val > 31) {
            logger_.error(param1->pos(), "bit position between 0 and 31.");
            return value_;
        }
    } else {
        logger_.error(param1->pos(), "constant expression expected.");
        return value_;
    }
    const auto base = builder_.getInt32Ty();
    const auto ptrtype = PointerType::get(base, 0);
    const auto ptr = builder_.CreateIntToPtr(params[0], ptrtype);
    const auto value = builder_.CreateLoad(base, ptr, true);
    Value *lhs = builder_.CreateLShr(value, params[1]);
    Value *rhs = ConstantInt::get(base, 0x00000001);
    Value *res = builder_.CreateAnd(lhs, rhs);
    return builder_.CreateICmpEQ(res, ConstantInt::get(base, 1));
}

Value *
LLVMIRBuilder::createSystemCopyCall(Value *src, Value *dst, Value *n) {
    const auto ptrtype = builder_.getPtrTy();
    const auto srcptr = builder_.CreateIntToPtr(src, ptrtype);
    const auto dstptr = builder_.CreateIntToPtr(dst, ptrtype);
    return builder_.CreateMemCpy(dstptr, {}, srcptr, {}, builder_.CreateShl(n, 2), true);
}

Value *
LLVMIRBuilder::createSystemValCall(const vector<unique_ptr<ExpressionNode>> &actuals, const vector<Value *> &params) {
    // TODO Introduce support further types: RECORD, etc.
    const auto dst = actuals[0].get();
    const auto decl = dynamic_cast<QualifiedExpression *>(dst)->dereference();
    const auto dsttype = dynamic_cast<TypeDeclarationNode *>(decl)->getType();
    const auto src = actuals[1].get();
    const auto srctype = src->getType();
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
    }
    return builder_.CreateTrunc(srcpar, getLLVMType(dsttype));
}

void LLVMIRBuilder::procedure(ProcedureNode &node) {
    const auto name = qualifiedName(&node);
    if (module_->getFunction(name)) {
        logger_.error(node.pos(), "Function " + name + " already defined.");
        return;
    }
    node.getType()->accept(*this);
    auto callee = module_->getOrInsertFunction(name, funTypes_[node.getType()]);
    const auto function = dyn_cast<Function>(callee.getCallee());
    if (triple_.isOSWindows() && node.getIdentifier()->isExported() && !config_.hasFlag(Flag::ENABLE_MAIN)) {
        function->setDLLStorageClass(GlobalValue::DLLStorageClassTypes::DLLExportStorageClass);
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

string LLVMIRBuilder::createScopedName(const TypeNode *type) const {
    if (!type->isAnonymous() && scopes_.size() <= 1) {
        return type->getIdentifier()->name();
    }
    ostringstream oss;
    size_t i = 0;
    string sep;
    while (i < scopes_.size()) {
        oss << sep << scopes_[i++];
        sep = "_";
        if (!type->isAnonymous() && i == scopes_.size() - 1) {
            return oss.str() + sep + type->getIdentifier()->name();
        }
    }
    return oss.str();
}

Value *LLVMIRBuilder::processGEP(Type *base, Value *value, vector<Value *> &indices) {
    if (indices.size() > 1) {
        const auto result = builder_.CreateInBoundsGEP(base, value, indices);
        indices.clear();
        indices.push_back(builder_.getInt32(0));
        return result;
    }
    return value;
}

Type* LLVMIRBuilder::getLLVMType(TypeNode *type) {
    if (type == nullptr) {
        return builder_.getVoidTy();
    }
    if (!types_[type]) {
        type->accept(*this);
    }
    return types_[type];
}

MaybeAlign LLVMIRBuilder::getLLVMAlign(TypeNode *type) {
    const auto layout = module_->getDataLayout();
    if (type->getNodeType() == NodeType::array_type) {
        const auto array_t = dynamic_cast<ArrayTypeNode*>(type);
        const auto int_align = layout.getPrefTypeAlign(builder_.getInt64Ty());
        const auto mem_align = getLLVMAlign(array_t->getMemberType());
        if (int_align.value() > mem_align->value()) {
            return int_align;
        }
        return mem_align;
    }
    if (type->getNodeType() == NodeType::record_type) {
        const auto record_t = dynamic_cast<RecordTypeNode *>(type);
        uint64_t size = 0;
        for (size_t i = 0; i < record_t->getFieldCount(); i++) {
            const auto field_t = record_t->getField(i)->getType();
            size = std::max(size, getLLVMAlign(field_t)->value());
        }
        return MaybeAlign(size);
    }
    if (type->getNodeType() == NodeType::pointer_type) {
        return { layout.getPointerPrefAlignment() };
    }
    if (type->getNodeType() == NodeType::basic_type) {
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

void LLVMIRBuilder::ensureTerminator(BasicBlock *block) {
    if (builder_.GetInsertBlock()->getTerminator() == nullptr) {
        builder_.CreateBr(block);
    }
}
