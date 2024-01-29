/*
 * Semantic analysis pass used by the analyzer of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/3/20.
 */

#include <set>
#include "SemanticAnalysis.h"

SemanticAnalysis::SemanticAnalysis(SymbolTable *symbols,
                                   SymbolImporter *importer, SymbolExporter *exporter) :
          Analysis(), NodeVisitor(), logger_(), symbols_(symbols), module_(), parent_(),
          importer_(importer), exporter_(exporter), forwards_() {
    tBoolean_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::BOOLEAN)));
    tByte_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::BYTE)));
    tChar_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::CHAR)));
    tInteger_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::INTEGER)));
    tReal_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::REAL)));
    tLongReal_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::LONGREAL)));
    tString_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::STRING)));
}

void SemanticAnalysis::run(Logger *logger, Node *node) {
    logger_ = logger;
    node->accept(*this);
}

void SemanticAnalysis::block(BlockNode &node) {
    auto parent = parent_;
    parent_ = &node;
    for (size_t cnt = 0; cnt < node.getConstantCount(); cnt++) {
        node.getConstant(cnt)->accept(*this);
    }
    forwards_.clear();
    for (size_t cnt = 0; cnt < node.getTypeDeclarationCount(); cnt++) {
        node.getTypeDeclaration(cnt)->accept(*this);
    }
    if (!forwards_.empty()) {
        for (auto pair: forwards_) {
            auto base = pair.second->getBase();
            logger_->error(base->pos(), "undefined forward reference: " + format(base) + ".");
        }
    }
    for (size_t cnt = 0; cnt < node.getVariableCount(); cnt++) {
        node.getVariable(cnt)->accept(*this);
    }
    for (size_t cnt = 0; cnt < node.getProcedureCount(); cnt++) {
        node.getProcedure(cnt)->accept(*this);
    }
    node.getStatements()->accept(*this);
    parent_ = parent;
}

void SemanticAnalysis::call(ProcedureNodeReference &node) {
    auto proc = dynamic_cast<ProcedureNode *>(node.dereference());
    if (node.ident()->isQualified() && proc->isExtern()) {
        // a fully-qualified external reference needs to added to module for code generation
        module_->addExternalProcedure(proc);
    }
    if (node.getActualParameterCount() < proc->getFormalParameterCount()) {
        logger_->error(node.pos(), "fewer actual than formal parameters.");
    }
    for (size_t cnt = 0; cnt < node.getActualParameterCount(); cnt++) {
        auto expr = node.getActualParameter(cnt);
        expr->accept(*this);
        if (cnt < proc->getFormalParameterCount()) {
            auto parameter = proc->getFormalParameter(cnt);
            if (assertCompatible(expr->pos(), parameter->getType(), expr->getType())) {
                if (parameter->isVar()) {
                    if (expr->getNodeType() == NodeType::value_reference) {
                        auto reference = dynamic_cast<const ValueReferenceNode *>(expr);
                        auto value = reference->dereference();
                        if (value->getNodeType() == NodeType::constant) {
                            logger_->error(expr->pos(),
                                           "illegal actual parameter: cannot pass constant by reference.");
                        }
                    } else {
                        logger_->error(expr->pos(),
                                       "illegal actual parameter: cannot pass expression by reference.");
                    }
                }
            }
            // Casting
            if (parameter->getType() != expr->getType()) {
                expr->setCast(parameter->getType());
            }
        } else if (!proc->hasVarArgs()) {
            logger_->error(expr->pos(), "more actual than formal parameters.");
            break;
        }
        // Folding
        if (expr->isConstant()) {
            node.setActualParameter(cnt, fold(expr));
        }
    }
}

void SemanticAnalysis::visit(ModuleNode &node) {
    auto name = node.getIdentifier()->name();
    symbols_->createNamespace(name, true);
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    module_ = &node;
    for (size_t i = 0; i < node.getImportCount(); i++) {
        node.getImport(i)->accept(*this);
    }
    symbols_->openScope();
    block(node);
    symbols_->closeScope();
    if (logger_->getErrorCount() == 0) {
        exporter_->write(name, symbols_);
    }
}

void SemanticAnalysis::visit(ImportNode &node) {
    // TODO check duplicate imports
    std::unique_ptr<ModuleNode> module;
    if (node.getAlias()) {
        module = importer_->read(node.getAlias()->name(), node.getModule()->name(), symbols_);
    } else {
        module = importer_->read(node.getModule()->name(), symbols_);
    }
    if (module) {
        module_->addExternalModule(std::move(module));
    } else {
        logger_->error(node.pos(), "module " + node.getModule()->name() + " could not be imported.");
    }
}

void SemanticAnalysis::visit(ConstantDeclarationNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    auto expr = node.getValue();
    if (expr) {
        expr->accept(*this);
        auto value = fold(expr);
        if (value) {
            if (value->isConstant()) {
                node.setType(value->getType());
                node.setValue(std::move(value));
            } else {
                logger_->error(value->pos(), "value must be constant.");
            }
        } else {
            logger_->error(node.pos(), "undefined constant.");
        }
    } else {
        logger_->error(node.pos(), "undefined constant.");
    }
}

void SemanticAnalysis::visit(TypeDeclarationNode &node) {
    auto ident = node.getIdentifier();
    assertUnique(ident, node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    auto type = node.getType();
    if (type) {
        type->accept(*this);
        // TODO check for infinite recursive type
        node.setType(resolveType(type));
    } else {
        logger_->error(node.pos(), "undefined type");
    }
    auto it = forwards_.find(ident->name());
    if (it != forwards_.end()) {
        auto pointer_t = it->second;
        logger_->debug("Resolving forward reference " + to_string(*ident));
        pointer_t->accept(*this);
        forwards_.erase(it);
    }
}

void SemanticAnalysis::visit(TypeReferenceNode &node) {
    auto sym = symbols_->lookup(node.getIdentifier());
    if (sym == nullptr) {
        logger_->error(node.pos(), "undefined type: " + to_string(*node.getIdentifier()) + ".");
    } else if (sym->getNodeType() == NodeType::array_type ||
               sym->getNodeType() == NodeType::basic_type ||
               sym->getNodeType() == NodeType::record_type ||
               sym->getNodeType() == NodeType::pointer_type) {
        node.resolve(dynamic_cast<TypeNode *>(sym));
    } else if (sym->getNodeType() == NodeType::type_declaration) {
        auto type = dynamic_cast<TypeDeclarationNode *>(sym);
        node.resolve(type->getType());
    } else {
        logger_->error(node.pos(), to_string(*node.getIdentifier()) + " is not a type.");
    }
}


void SemanticAnalysis::visit(VariableDeclarationNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    auto type = node.getType();
    if (type) {
        // type only needs to be checked for the first variable in a variable list
        if (node.index() == 0) {
            type->accept(*this);
        }
    } else {
        logger_->error(node.pos(), "undefined variable type.");
    }
    node.setType(resolveType(type));
}

void SemanticAnalysis::visit(ProcedureNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    symbols_->openScope();
    for (size_t i = 0; i < node.getFormalParameterCount(); i++) {
        node.getFormalParameter(i)->accept(*this);
    }
    auto type = node.getReturnType();
    if (type) {
        type->accept(*this);
        node.setReturnType(resolveType(type));
    }
    block(node);
    symbols_->closeScope();
    if (node.isExtern() && node.getLevel() != SymbolTable::MODULE_LEVEL) {
        logger_->error(node.pos(), "only top-level procedures can be external.");
    }
}

void SemanticAnalysis::visit(ParameterNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    auto type = node.getType();
    if (type) {
        // type only needs to be checked for the first parameter in a parameter list
        if (node.index() == 0) {
            type->accept(*this);
        }
        node.setType(resolveType(type));
    } else {
        logger_->error(node.pos(), "undefined parameter type.");
    }
}

void SemanticAnalysis::visit(FieldNode &node) {
    auto type = node.getType();
    if (type) {
        // type only needs to be checked for the first field in a field list
        if (node.index() == 0) {
            type->accept(*this);
        }
        node.setType(resolveType(type));
    } else {
        logger_->error(node.pos(), "undefined record field type.");
    }
}

void SemanticAnalysis::visit(ValueReferenceNode &node) {
    auto ident = node.ident();
    auto sym = symbols_->lookup(ident);
    if (!sym && ident->isQualified()) {
        // addresses the fact that 'ident.ident' is ambiguous: 'qual.ident' vs. 'ident.field'.
        auto qual = dynamic_cast<QualIdent *>(ident);
        sym = symbols_->lookup(qual->qualifier());
        if (sym) {
            node.disqualify();
        }
    }
    if (sym) {
        if (sym->getNodeType() == NodeType::constant ||
            sym->getNodeType() == NodeType::parameter ||
            sym->getNodeType() == NodeType::variable ||
            sym->getNodeType() == NodeType::procedure) {
            auto decl = dynamic_cast<DeclarationNode *>(sym);
            node.resolve(decl);
            if (node.getNodeType() == NodeType::procedure_call) {
                call(node);
            }
            auto type = decl->getType();
            if (!type) {
                if (sym->getNodeType() == NodeType::procedure) {
                    logger_->error(node.pos(), "function expected, found procedure.");
                    return;
                }
                logger_->error(node.pos(), "type undefined for " + to_string(*node.ident()) + ".");
                return;
            }
            size_t pos = 0;
            size_t last = node.getSelectorCount();
            while (pos < last) {
                auto sel = node.getSelector(pos);
                if (type->getNodeType() != sel->getType()) {
                    if (type->getNodeType() == NodeType::pointer_type &&
                                (sel->getType() == NodeType::array_type || sel->getType() == NodeType::record_type)) {
                        // perform implicit pointer dereferencing
                        auto caret = std::make_unique<Dereference>(EMPTY_POS);
                        sel = caret.get();
                        node.insertSelector(pos, std::move(caret));
                        last++;
                    } else if (sel->getType() == NodeType::type_declaration) {
                        // handle type-guard or clean up parsing ambiguity
                        auto guard = dynamic_cast<Typeguard *>(sel);
                        sym = symbols_->lookup(guard->ident());
                        if (sym) {
                            if (sym->getNodeType() == NodeType::type_declaration) {
                                type = dynamic_cast<TypeDeclarationNode *>(sym)->getType();
                            } else if (sym->getNodeType() == NodeType::basic_type ||
                                       sym->getNodeType() == NodeType::array_type ||
                                       sym->getNodeType() == NodeType::record_type ||
                                       sym->getNodeType() == NodeType::pointer_type) {
                                type = dynamic_cast<TypeNode*>(sym);
                            } else {
                                logger_->error(sel->pos(), "oh, oh, parser has mistaken function call as type-guard.");
                            }
                        } else {
                            logger_->error(node.pos(), "undefined identifier: " + to_string(*guard->ident()) + ".");
                        }
                    } else {
                        logger_->error(sel->pos(), "selector type mismatch.");
                    }
                }
                if (sel->getType() == NodeType::array_type && type->isArray()) {
                    auto array_t = dynamic_cast<ArrayTypeNode *>(type);
                    auto expr = dynamic_cast<ArrayIndex*>(sel)->getExpression();
                    expr->accept(*this);
                    auto sel_type = expr->getType();
                    if (sel_type && sel_type->kind() != TypeKind::INTEGER) {
                        logger_->error(sel->pos(), "integer expression expected.");
                    }
                    if (expr->isConstant()) {
                        auto value = fold(expr);
                        if (value) {
                            node.setSelector(pos, std::make_unique<ArrayIndex>(EMPTY_POS, std::move(value)));
                        }
                    }
                    type = array_t->getMemberType();
                } else if (sel->getType() == NodeType::record_type && type->isRecord()) {
                    auto record_t = dynamic_cast<RecordTypeNode *>(type);
                    auto ref = dynamic_cast<RecordField *>(sel);
                    auto field = record_t->getField(ref->ident()->name());
                    if (field) {
                        ref->setField(field);
                        type = field->getType();
                    } else {
                        logger_->error(ref->pos(),
                                       "unknown record field: " + to_string(*ref->ident()) + ".");
                    }
                } else if (sel->getType() == NodeType::pointer_type && type->isPointer()) {
                    auto pointer_t = dynamic_cast<PointerTypeNode *>(type);
                    type = pointer_t->getBase();
                } else if (sel->getType() == NodeType::type_declaration) {
                    // nothing to do here as type-guard is handled above
                } else {
                    logger_->error(sel->pos(), "unexpected selector.");
                }
                pos++;
            }
            node.setType(resolveType(type));
        } else {
            logger_->error(node.pos(), "constant, parameter, variable, function call expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined identifier: " + to_string(*node.ident()) + ".");
    }
}


void SemanticAnalysis::visit(BooleanLiteralNode &node) {
    node.setType(this->tBoolean_);
}

void SemanticAnalysis::visit(IntegerLiteralNode &node) {
    node.setType(this->tInteger_);
}

void SemanticAnalysis::visit(RealLiteralNode &node) {
    node.setType(this->tReal_);
}


void SemanticAnalysis::visit(StringLiteralNode &node) {
    node.setType(this->tString_);
}

void SemanticAnalysis::visit(NilLiteralNode &node) {
    node.setType(symbols_->getNilType());
}

void SemanticAnalysis::visit(UnaryExpressionNode &node) {
    auto expr = node.getExpression();
    if (expr) {
        expr->accept(*this);
        if (expr->isConstant()) {
            auto value = fold(expr);
            if (value) {
                node.setExpression(std::move(value));
            }
        }
        node.setType(node.getExpression()->getType());
    } else {
        logger_->error(node.pos(), "undefined expression.");
    }
}

void SemanticAnalysis::visit(BinaryExpressionNode &node) {
    auto lhs = node.getLeftExpression();
    if (lhs) {
        lhs->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined left-hand side in expression.");
        return;
    }
    auto rhs = node.getRightExpression();
    if (rhs) {
        rhs->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined right-hand side in expression.");
        return;
    }
    auto lhsType = lhs->getType();
    if (!lhsType) {
        logger_->error(lhs->pos(), "undefined left-hand side type in expression.");
        return;
    }
    auto rhsType = rhs->getType();
    if (!rhsType) {
        logger_->error(lhs->pos(), "undefined right-hand side type in expression.");
        return;
    }
    // Type inference
    auto op = node.getOperator();
    auto common = commonType(lhsType, rhsType);
    if (common) {
        if (op == OperatorType::EQ
            || op == OperatorType::NEQ
            || op == OperatorType::LT
            || op == OperatorType::LEQ
            || op == OperatorType::GT
            || op == OperatorType::GEQ) {
            node.setType(this->tBoolean_);
        } else if (op == OperatorType::DIV) {
            if (lhsType->isInteger() && rhsType->isInteger()) {
                node.setType(common);
            } else {
                logger_->error(node.pos(), "integer division needs integer arguments.");
            }
        } else if (op == OperatorType::DIVIDE) {
            if (common->isInteger()) {
                if (common->kind() == TypeKind::LONGINT) {
                    node.setType(this->tLongReal_);
                } else {
                    node.setType(this->tReal_);
                }
            } else {
                node.setType(common);
            }
        } else {
            node.setType(common);
        }
    } else {
        logger_->error(node.pos(), "incompatible types (" + lhsType->getIdentifier()->name() + ", " +
                                   rhsType->getIdentifier()->name() + ")");
    }
    // Folding
    if (lhs->isConstant() && !lhs->isLiteral()) {
        node.setLeftExpression(fold(lhs));
    }
    if (rhs->isConstant() && !rhs->isLiteral()) {
        node.setRightExpression(fold(rhs));
    }
    // Casting
    if (common) {
        if (node.getLeftExpression()->getType() != common) {
            node.getLeftExpression()->setCast(common);
        }
        if (node.getRightExpression()->getType() != common) {
            node.getRightExpression()->setCast(common);
        }
    }
}

void SemanticAnalysis::visit([[maybe_unused]] ArrayTypeNode &node) {
//    auto expr = node.getExpression();
//    if (expr) {
//        expr->accept(*this);
//        if (expr->isConstant()) {
//            auto type = expr->getType();
//            if (type && type->kind() == TypeKind::INTEGER) {
//                auto dim = foldInteger(expr);
//                if (dim > 0) {
//                    node.setDimension((unsigned int) dim);
//                } else {
//                    logger_->error(expr->pos(), "array dimension must be a positive value.");
//                }
//            } else {
//                logger_->error(expr->pos(), "integer expression expected.");
//            }
//        } else {
//            logger_->error(expr->pos(), "constant expression expected.");
//        }
//    } else {
//        logger_->error(node.pos(), "undefined array type.");
//    }
//    auto type = node.getMemberType();
//    if (type) {
//        type->accept(*this);
//        node.setMemberType(resolveType(type));
//    } else {
//        logger_->error(node.pos(), "undefined type.");
//    }
//    if (type && type->getSize() > 0 && node.getDimension() > 0) {
//        node.setSize(node.getDimension() * type->getSize());
//    } else {
//        logger_->error(node.pos(), "undefined array dimension.");
//    }
}

void SemanticAnalysis::visit([[maybe_unused]] BasicTypeNode &node) {}

void SemanticAnalysis::visit([[maybe_unused]] ProcedureTypeNode &node) {}

void SemanticAnalysis::visit(RecordTypeNode &node) {
    std::set<std::string> fields;
    if (node.getFieldCount() > 0) {
        for (size_t i = 0; i < node.getFieldCount(); i++) {
            auto field = node.getField(i);
            if (field->getIdentifier()->isExported()) {
                if (symbols_->getLevel() != SymbolTable::MODULE_LEVEL) {
                    logger_->error(field->pos(), "only top-level declarations can be exported.");
                } else if (!node.getIdentifier()->isExported()) {
                    logger_->error(field->pos(), "cannot export fields of non-exported record type.");
                }
            }
            field->accept(*this);
            if (fields.count(field->getIdentifier()->name())) {
                logger_->error(field->pos(), "duplicate record field: " + to_string(*field->getIdentifier()) + ".");
            }
        }
    } else {
        logger_->error(node.pos(), "records needs at least one field.");
    }
}

void SemanticAnalysis::visit(PointerTypeNode &node) {
    auto type = node.getBase();
    if (type) {
        if (type->getNodeType() == NodeType::type_reference) {
            auto sym = symbols_->lookup(type->getIdentifier());
            if (sym == nullptr) {
                auto ident = type->getIdentifier();
                forwards_[ident->name()] = &node;
                logger_->debug("Found possible forward type reference: " + to_string(*ident));
            } else {
                type->accept(*this);
                node.setBase(resolveType(type));
            }
        } else {
            type->accept(*this);
        }
    } else {
        logger_->error(node.pos(), "undefined type.");
    }
}

void SemanticAnalysis::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        auto statement = node.getStatement(i);
        if (statement) {
            node.getStatement(i)->accept(*this);
        }
    }
}

void SemanticAnalysis::visit(AssignmentNode &node) {
    auto lvalue = node.getLvalue();
    if (lvalue) {
        lvalue->accept(*this);
        auto decl = lvalue->dereference();
        if (decl) {
//            if (decl->getNodeType() == NodeType::parameter) {
//                auto param = dynamic_cast<ParameterNode *>(lvalue->dereference());
//                if (!param->isVar()) {
//                    logger_->error(lvalue->pos(), "cannot assign non-var parameter.");
//                }
//            }
            if (lvalue->dereference()->getNodeType() == NodeType::constant) {
                logger_->error(lvalue->pos(), "cannot assign constant.");
            }
        } else {
            logger_->error(node.pos(), "undefined left-hand side in assignment.");
        }
    } else {
        logger_->error(node.pos(), "undefined left-hand side in assignment.");
    }
    auto rvalue = node.getRvalue();
    if (rvalue) {
        rvalue->accept(*this);
        if (lvalue && assertCompatible(lvalue->pos(), lvalue->getType(), rvalue->getType())) {
            // Casting right-hand side to type expected by left-hand side
            if (lvalue->getType() != rvalue->getType()) {
                rvalue->setCast(lvalue->getType());
            }
            // Casting NIL to expected type
            if (rvalue->getType()->kind() == TypeKind::NILTYPE) {
                rvalue->setCast(lvalue->getType());
            }
            // Folding constant expressions, if they are not literals
            if (rvalue->isConstant() && !rvalue->isLiteral()) {
                node.setRvalue(fold(rvalue));
            }

        }
    } else {
        logger_->error(node.pos(), "undefined right-hand side in assignment.");
    }
}

void SemanticAnalysis::visit(IfThenElseNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in if-statement.");
    }
    node.getThenStatements()->accept(*this);
    if (node.hasElseIf()) {
        for (size_t i = 0; i < node.getElseIfCount(); i++) {
            node.getElseIf(i)->accept(*this);
        }
    }
    if (node.hasElse()) {
        node.getElseStatements()->accept(*this);
    }
}

void SemanticAnalysis::visit(ElseIfNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in elsif-statement.");
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(ProcedureCallNode &node) {
    auto symbol = symbols_->lookup(node.ident());
    if (symbol) {
        if (symbol->getNodeType() == NodeType::procedure) {
            auto proc = dynamic_cast<ProcedureNode *>(symbol);
            node.resolve(proc);
            call(node);
        } else {
            logger_->error(node.pos(), to_string(*node.ident()) + " is not a procedure.");
        }
    } else {
        logger_->error(node.pos(), "undefined identifier: " + to_string(*node.ident()) + ".");
    }
}

void SemanticAnalysis::visit(LoopNode &node) {
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(WhileLoopNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in while-loop.");
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(RepeatLoopNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in repeat-loop.");
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(ForLoopNode &node) {
    auto counter = node.getCounter();
    if (counter) {
        counter->accept(*this);
        if (counter->getNodeType() == NodeType::value_reference) {
            auto ref = dynamic_cast<ValueReferenceNode *>(counter);
            auto var = ref->dereference();
            if (var->getNodeType() != NodeType::variable) {
                logger_->error(var->pos(), "variable expected.");
            }
            auto type = var->getType();
            if (type && type->kind() != TypeKind::INTEGER) {
                logger_->error(var->pos(), "integer variable expected.");
            }
        } else {
            logger_->error(counter->pos(), to_string(*counter->ident()) + " cannot be used as a loop counter.");
        }
    } else {
        logger_->error(node.pos(), "undefined counter variable in for-loop.");
    }
    auto low = node.getLow();
    if (low) {
        low->accept(*this);
        auto type = low->getType();
        if (type && type->kind() != TypeKind::INTEGER) {
            logger_->error(low->pos(), "integer expression expected.");
        }
        if (low->isConstant()) {
            node.setLow(fold(low));
        }
    } else {
        logger_->error(node.pos(), "undefined low value in for-loop.");
    }
    auto high = node.getHigh();
    if (high) {
        high->accept(*this);
        auto type = high->getType();
        if (type && type->kind() != TypeKind::INTEGER) {
            logger_->error(high->pos(), "integer expression expected.");
        }
        if (high->isConstant()) {
            node.setHigh(fold(high));
        }
    } else {
        logger_->error(node.pos(), "undefined high value in for-loop.");
    }
    auto expr = node.getStep();
    if (expr) {
        expr->accept(*this);
        auto type = expr->getType();
        if (type && type->kind() == TypeKind::INTEGER && expr->isConstant()) {
            auto step = foldInteger(expr);
            if (step == 0) {
                logger_->error(expr->pos(), "step value cannot be zero.");
            }
            node.setStep(std::make_unique<IntegerLiteralNode>(expr->pos(), step));
        } else {
            logger_->error(expr->pos(), "constant integer expression expected.");
        }
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(ReturnNode &node) {
    auto expr = node.getValue();
    if (expr) {
        expr->accept(*this);
    }
    if (parent_->getNodeType() == NodeType::module) {
        if (expr) {
            logger_->error(expr->pos(), "module cannot return a value.");
        }
    }
    if (parent_->getNodeType() == NodeType::procedure) {
        auto proc = dynamic_cast<ProcedureNode *>(parent_);
        if (expr) {
            if (!proc->getReturnType()) {
                logger_->error(expr->pos(), "procedure cannot return a value.");
            } else {
                assertCompatible(expr->pos(), proc->getReturnType(), expr->getType());
            }
        } else {
            if (proc->getReturnType()) {
                logger_->error(node.pos(), "function must return value.");
            }
        }
    }
}

bool SemanticAnalysis::assertEqual(Ident *aIdent, Ident *bIdent) const {
    if (aIdent == nullptr || bIdent == nullptr) {
        return false;
    }
    if (aIdent->name() == bIdent->name()) {
        if (aIdent->isQualified() == bIdent->isQualified()) {
            if (aIdent->isQualified()) {
                return dynamic_cast<QualIdent *>(aIdent)->qualifier() ==
                       dynamic_cast<QualIdent *>(bIdent)->qualifier();
            }
            return true;
        }
        return false;
    }
    return false;
}

void SemanticAnalysis::assertUnique(Ident *ident, Node &node) {
    if (ident->isQualified()) {
        logger_->error(ident->pos(), "cannot use qualified identifier here.");
    }
    if (symbols_->isDuplicate(ident->name())) {
        logger_->error(ident->pos(), "duplicate definition: " + ident->name() + ".");
    }
    if (symbols_->isGlobal(ident->name())) {
        logger_->error(ident->pos(), "predefined identifier: " + ident->name() + ".");
    }
    symbols_->insert(ident->name(), &node);
}

bool SemanticAnalysis::assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual, bool isPtr) {
    if (expected == actual) {
        return true;
    } else if (expected && actual) {
        if (expected->kind() == TypeKind::ANYTYPE || expected->kind() == TypeKind::ANYTYPE) {
            return true;
        }
        auto expectedId = expected->getIdentifier();
        auto actualId = actual->getIdentifier();
        if (assertEqual(expectedId, actualId)) {
            // the two types are the same type
            return true;
        } else if ((expected->isInteger() && actual->isInteger()) ||
                   (expected->isReal() && actual->isNumeric())) {
            if (expected->getSize() < actual->getSize()) {
                logger_->error(pos, "type mismatch: converting " + to_string(*actualId) +
                                    " to " + to_string(*expectedId) + " may lose data.");
                return false;
            }
            return true;
        } else if (expected->isArray() && actual->isArray()) {
            auto exp_array = dynamic_cast<ArrayTypeNode *>(expected);
            auto act_array = dynamic_cast<ArrayTypeNode *>(actual);
            if ((exp_array->isOpen() || exp_array->getDimension() == act_array->getDimension())) {
                if (exp_array->getMemberType()->kind() == TypeKind::ANYTYPE) {
                    return true;
                }
                auto exp_mem_t = exp_array->getMemberType();
                auto act_mem_t = act_array->getMemberType();
                if (exp_mem_t && act_mem_t &&
                    assertEqual(exp_mem_t->getIdentifier(), act_mem_t->getIdentifier())) {
                    return true;
                }
            }
            std::cerr << ">>>>>>>" << std::endl;
        } else if (expected->isPointer()) {
            if (actual->isPointer()) {
                auto exp_ptr = dynamic_cast<PointerTypeNode *>(expected);
                auto act_ptr = dynamic_cast<PointerTypeNode *>(actual);
                return assertCompatible(pos, exp_ptr->getBase(), act_ptr->getBase(), true);
            } else if (actual->kind() == TypeKind::NILTYPE) {
                return true;
            }
        }
        // error logging
        logger_->error(pos, "type mismatch: expected " + format(expected, isPtr) +
                            ", found " + format(actual, isPtr) + ".");
        return false;
    } else {
        logger_->error(pos, "type mismatch.");
        return false;
    }
}

void SemanticAnalysis::checkExport(DeclarationNode &node) {
    if (node.getLevel() != SymbolTable::MODULE_LEVEL && node.getIdentifier()->isExported()) {
        logger_->error(node.getIdentifier()->pos(), "only top-level declarations can be exported.");
    }
}

std::unique_ptr<LiteralNode> SemanticAnalysis::fold(const ExpressionNode *expr) const {
    auto type = expr->getType();
    if (type) {
        auto cast = expr->getCast();
        if (type->kind() == TypeKind::INTEGER || type->kind() == TypeKind::LONGINT) {
            return std::make_unique<IntegerLiteralNode>(expr->pos(), foldInteger(expr), type, cast);
        } else if (type->kind() == TypeKind::REAL || type->kind() == TypeKind::LONGREAL) {
            return std::make_unique<RealLiteralNode>(expr->pos(), foldReal(expr), type, cast);
        } else if (type->kind() == TypeKind::BOOLEAN) {
            return std::make_unique<BooleanLiteralNode>(expr->pos(), foldBoolean(expr), type, cast);
        } else if (type->kind() == TypeKind::STRING) {
            return std::make_unique<StringLiteralNode>(expr->pos(), foldString(expr), type, cast);
        }
        logger_->error(expr->pos(), "incompatible types.");
    } else {
        logger_->error(expr->pos(), "undefined type.");
    }
    return nullptr;
}

long SemanticAnalysis::foldInteger(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode *>(expr);
        auto value = foldInteger(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NEG:
                return -value;
            case OperatorType::PLUS:
                return value;
            default:
                logger_->error(unExpr->pos(), "incompatible unary operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode *>(expr);
        auto lValue = foldInteger(binExpr->getLeftExpression());
        auto rValue = foldInteger(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:
                return lValue + rValue;
            case OperatorType::MINUS:
                return lValue - rValue;
            case OperatorType::TIMES:
                return lValue * rValue;
            case OperatorType::DIV:
                return lValue / rValue;
            case OperatorType::MOD:
                return lValue % rValue;
            default:
                logger_->error(binExpr->pos(), "incompatible binary operator.");
        }
    } else if (expr->getNodeType() == NodeType::integer) {
        auto integer = dynamic_cast<const IntegerLiteralNode *>(expr);
        return integer->value();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return foldInteger(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression.");
    }
    return 0;
}

double SemanticAnalysis::foldReal(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode *>(expr);
        auto value = foldReal(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NEG:
                return -value;
            case OperatorType::PLUS:
                return value;
            default:
                logger_->error(unExpr->pos(), "incompatible unary operator.");
        }

    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode *>(expr);
        auto lValue = foldReal(binExpr->getLeftExpression());
        auto rValue = foldReal(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:
                return lValue + rValue;
            case OperatorType::MINUS:
                return lValue - rValue;
            case OperatorType::TIMES:
                return lValue * rValue;
            case OperatorType::DIVIDE:
                return lValue / rValue;
            default:
                logger_->error(binExpr->pos(), "incompatible binary operator.");
        }
    } else if (expr->getNodeType() == NodeType::integer) {
        // promote integer to real
        auto integer = dynamic_cast<const IntegerLiteralNode *>(expr);
        return (double) integer->value();
    } else if (expr->getNodeType() == NodeType::real) {
        auto real = dynamic_cast<const RealLiteralNode *>(expr);
        return real->value();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return foldReal(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression.");
    }
    return 0.0;
}

bool SemanticAnalysis::foldBoolean(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode *>(expr);
        bool value = foldBoolean(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NOT:
                return !value;
            default:
                logger_->error(unExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode *>(expr);
        OperatorType op = binExpr->getOperator();
        auto lhs = binExpr->getLeftExpression();
        auto rhs = binExpr->getRightExpression();
        auto type = lhs->getType();
        if (type->kind() == TypeKind::BOOLEAN) {
            bool lValue = foldBoolean(binExpr->getLeftExpression());
            bool rValue = foldBoolean(binExpr->getRightExpression());
            switch (op) {
                case OperatorType::AND:
                    return lValue && rValue;
                case OperatorType::OR:
                    return lValue || rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else if (type->kind() == TypeKind::INTEGER) {
            int lValue = foldInteger(lhs);
            int rValue = foldInteger(rhs);
            switch (op) {
                case OperatorType::EQ:
                    return lValue == rValue;
                case OperatorType::NEQ:
                    return lValue != rValue;
                case OperatorType::LT:
                    return lValue < rValue;
                case OperatorType::LEQ:
                    return lValue <= rValue;
                case OperatorType::GT:
                    return lValue > rValue;
                case OperatorType::GEQ:
                    return lValue >= rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else if (type->kind() == TypeKind::STRING) {
            std::string lValue = foldString(lhs);
            std::string rValue = foldString(rhs);
            switch (op) {
                case OperatorType::EQ:
                    return lValue == rValue;
                case OperatorType::NEQ:
                    return lValue != rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else {
            logger_->error(expr->pos(), "incompatible expression.");
        }
    } else if (expr->getNodeType() == NodeType::boolean) {
        auto boolConst = dynamic_cast<const BooleanLiteralNode *>(expr);
        return boolConst->value();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return foldBoolean(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression.");
    }
    return false;
}

std::string SemanticAnalysis::foldString(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode *>(expr);
        std::string lValue = foldString(binExpr->getLeftExpression());
        std::string rValue = foldString(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:
                return std::string(lValue + rValue);
            default:
                logger_->error(binExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::string) {
        auto stringConst = dynamic_cast<const StringLiteralNode *>(expr);
        return stringConst->value();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return foldString(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression (string).");
    }
    return "";
}

std::string SemanticAnalysis::format(const TypeNode *type, bool isPtr) const {
    auto name = isPtr ? std::string("POINTER TO ") : std::string();
    if (type->getIdentifier()) {
        name += to_string(*type->getIdentifier());
    } else {
        std::stringstream stream;
        stream << *type;
        name += stream.str();
    }
    return name;
}


TypeNode *SemanticAnalysis::commonType(TypeNode *lhsType, TypeNode *rhsType) const {
    if (lhsType == rhsType) {
        return lhsType;
    } else if (assertEqual(lhsType->getIdentifier(), rhsType->getIdentifier())) {
        return lhsType;
    } else if (lhsType->isNumeric() && rhsType->isNumeric()) {
        if (lhsType->getSize() == rhsType->getSize()) {
            return lhsType->isReal() ? lhsType : rhsType;
        } else {
            return lhsType->getSize() > rhsType->getSize() ? lhsType : rhsType;
        }
    } else if (lhsType->kind() == TypeKind::NILTYPE) {
        return rhsType;
    } else if (rhsType->kind() == TypeKind::NILTYPE) {
        return lhsType;
    }
    return nullptr;
}

TypeNode *SemanticAnalysis::resolveType(TypeNode *type) {
    if (type == nullptr) {
        return type;
    }
    if (type->getNodeType() == NodeType::type_reference) {
        auto ref = dynamic_cast<TypeReferenceNode *>(type);
        if (ref->isResolved()) {
            return ref->dereference();
        } else {
            logger_->error(ref->pos(), "unresolved type reference: " + to_string(*ref->getIdentifier()) + ".");
        }
    }
    return type;
}