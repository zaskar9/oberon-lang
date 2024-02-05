//
// Created by Michael Grossniklaus on 12/17/23.
//

#include "Sema.h"

#include <memory>
#include <string>

using std::make_unique;
using std::unique_ptr;
using std::string;

Sema::Sema(ASTContext *context, SymbolTable *symbols, SymbolImporter *importer, SymbolExporter *exporter, Logger *logger) :
        context_(context), symbols_(symbols), importer_(importer), exporter_(exporter), logger_(logger) {
    tBoolean_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::BOOLEAN)));
    tByte_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::BYTE)));
    tChar_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::CHAR)));
    tInteger_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::INTEGER)));
    tLongInt_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::LONGINT)));
    tReal_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::REAL)));
    tLongReal_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::LONGREAL)));
    tString_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::STRING)));
}

void
Sema::onTranslationUnitStart(const string &name) {
    symbols_->createNamespace(name, true);
}

void
Sema::onTranslationUnitEnd(const string &name) {
    if (logger_->getErrorCount() == 0) {
        exporter_->write(name, symbols_);
    }
}

unique_ptr<ModuleNode>
Sema::onModule(const FilePos &start, [[maybe_unused]] const FilePos &end,
               unique_ptr<Ident> ident,
               vector<unique_ptr<ImportNode>> imports,
               vector<unique_ptr<ConstantDeclarationNode>> consts, vector<unique_ptr<TypeDeclarationNode>> types,
               vector<unique_ptr<VariableDeclarationNode>> vars, vector<unique_ptr<ProcedureNode>> procs,
               unique_ptr<StatementSequenceNode> stmts) {
    return make_unique<ModuleNode>(start, std::move(ident), std::move(imports),
                                   std::move(consts), std::move(types), std::move(vars), std::move(procs),
                                   std::move(stmts));
}

unique_ptr<ConstantDeclarationNode>
Sema::onConstant(const FilePos &start, [[maybe_unused]] const FilePos &end,
                 unique_ptr<IdentDef> ident, unique_ptr<ExpressionNode> expr) {
    if (!expr) {
        logger_->error(start, "undefined constant.");
        return nullptr;
    }
    if (!expr->isConstant()) {
        logger_->error(expr->pos(), "value must be constant.");
        return nullptr;
    }
    if (!expr->isLiteral()) {
        logger_->error(expr->pos(), "undefined constant.");
        return nullptr;
    }
    auto node = make_unique<ConstantDeclarationNode>(start, std::move(ident), std::move(expr));
    assertUnique(node->getIdentifier(), node.get());
    node->setLevel(symbols_->getLevel());
    checkExport(node.get());
    return node;
}

ArrayTypeNode *
Sema::onArrayType(const FilePos &start, [[maybe_unused]] const FilePos &end,
                  Ident *ident, unique_ptr<ExpressionNode> expr, TypeNode *type) {
    if (!expr) {
        logger_->error(start, "undefined array dimension.");
        return nullptr;
    }
    if (!expr->isConstant()) {
        logger_->error(expr->pos(), "constant expression expected.");
        return nullptr;
    }
    auto dim_t = expr->getType();
    if (!dim_t) {
        logger_->error(start, "undefined array dimension type.");
        return nullptr;
    }
    if (dim_t->kind() != TypeKind::INTEGER) {
        logger_->error(expr->pos(), "integer expression expected.");
        return nullptr;
    }
    if (!expr->isLiteral()) {
        logger_->error(expr->pos(), "this should not happen.");
        return nullptr;
    }
    auto dim = dynamic_cast<const IntegerLiteralNode*>(expr.get());
    if (dim->value() <= 0) {
        logger_->error(expr->pos(), "array dimension must be a positive value.");
        return nullptr;
    }
    if (!type) {
        logger_->error(start, "undefined member type.");
        return nullptr;
    }
    auto mem_t = resolveType(type);
    if (mem_t) {
        auto res = context_->getOrInsertArrayType(ident, (unsigned int) dim->value(), mem_t);
        if (mem_t->getSize() > 0 && dim->value() > 0) {
            res->setSize(dim->value() * mem_t->getSize());
        } else {
            logger_->error(start, "undefined array dimension.");
        }
        return res;
    }
    return nullptr;
}

PointerTypeNode *
Sema::onPointerType([[maybe_unused]] const FilePos &start, [[maybe_unused]] const FilePos &end,
                                     Ident *ident, TypeNode *base) {
    return context_->getOrInsertPointerType(ident, base);
}

ProcedureTypeNode *
Sema::onProcedureType([[maybe_unused]] const FilePos &start, [[maybe_unused]] const FilePos &end,
                      Ident *ident, vector<unique_ptr<ParameterNode>> params, TypeNode *ret) {
    return context_->getOrInsertProcedureType(ident, std::move(params), ret);
}

RecordTypeNode *
Sema::onRecordType([[maybe_unused]] const FilePos &start, [[maybe_unused]] const FilePos &end,
                  Ident *ident, vector<unique_ptr<FieldNode>> fields) {
    return context_->getOrInsertRecordType(ident, std::move(fields));
}

TypeNode *
Sema::onTypeReference(const FilePos &start, [[maybe_unused]] const FilePos &end, unique_ptr<QualIdent> ident) {
    auto sym = symbols_->lookup(ident.get());
    if (!sym) {
        // TODO on completion of new Sema: activate error and return nullptr
        // logger_->error(start, "undefined type: " + to_string(*ident) + ".");
        return context_->getOrInsertTypeReference(std::move(ident));
    }
    if (sym->getNodeType() == NodeType::array_type ||
        sym->getNodeType() == NodeType::basic_type ||
        sym->getNodeType() == NodeType::record_type ||
        sym->getNodeType() == NodeType::pointer_type) {
        return dynamic_cast<TypeNode *>(sym);
    }
    if (sym->getNodeType() == NodeType::type_declaration) {
        auto type = dynamic_cast<TypeDeclarationNode *>(sym);
        return type->getType();
    }
    logger_->error(start, to_string(*ident) + " is not a type.");
    return nullptr;
}

unique_ptr<ProcedureNode>
Sema::onProcedure(const FilePos &start, [[maybe_unused]] const FilePos &end,
                  unique_ptr<IdentDef> ident,
                  ProcedureTypeNode *type,
                  vector<unique_ptr<ConstantDeclarationNode>> consts,
                  vector<unique_ptr<TypeDeclarationNode>> types,
                  vector<unique_ptr<VariableDeclarationNode>> vars,
                  vector<unique_ptr<ProcedureNode>> procs,
                  unique_ptr<StatementSequenceNode> stmts) {
    return make_unique<ProcedureNode>(start, std::move(ident), type, std::move(consts), std::move(types),
                                      std::move(vars), std::move(procs), std::move(stmts));
}

unique_ptr<IfThenElseNode>
Sema::onIfStatement(const FilePos &start, [[maybe_unused]] const FilePos &end,
                    unique_ptr<ExpressionNode> condition,
                    unique_ptr<StatementSequenceNode> thenStmts,
                    vector<unique_ptr<ElseIfNode>> elseIfs,
                    unique_ptr<StatementSequenceNode> elseStmts) {
    return make_unique<IfThenElseNode>(start, std::move(condition), std::move(thenStmts),
                                       std::move(elseIfs), std::move(elseStmts));
}

unique_ptr<ElseIfNode>
Sema::onElseIf(const FilePos &start, [[maybe_unused]] const FilePos &end,
               unique_ptr<ExpressionNode> condition,
               unique_ptr<StatementSequenceNode> stmts) {
    return make_unique<ElseIfNode>(start, std::move(condition), std::move(stmts));
}

unique_ptr<LoopNode>
Sema::onLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
             unique_ptr<StatementSequenceNode> stmts) {
    return make_unique<LoopNode>(start, std::move(stmts));
}

unique_ptr<RepeatLoopNode>
Sema::onRepeatLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
                   unique_ptr<ExpressionNode> cond, unique_ptr<StatementSequenceNode> stmts) {
    return make_unique<RepeatLoopNode>(start, std::move(cond), std::move(stmts));
}

unique_ptr<WhileLoopNode>
Sema::onWhileLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
                  unique_ptr<ExpressionNode> cond, unique_ptr<StatementSequenceNode> stmts) {
    return make_unique<WhileLoopNode>(start, std::move(cond), std::move(stmts));
}

unique_ptr<ForLoopNode>
Sema::onForLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
                unique_ptr<Ident> var,
                unique_ptr<ExpressionNode> low, unique_ptr<ExpressionNode> high, unique_ptr<ExpressionNode> step,
                unique_ptr<StatementSequenceNode> stmts) {
    auto counter = make_unique<ValueReferenceNode>(var->start(), make_unique<Designator>(std::move(var)));
    return make_unique<ForLoopNode>(start, std::move(counter), std::move(low), std::move(high), std::move(step),
                                    std::move(stmts));
}

unique_ptr<ExpressionNode>
Sema::onUnaryExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                        OperatorType op, unique_ptr<ExpressionNode> expr) {
    if (!expr) {
        logger_->error(start, "undefined expression in unary expression.");
        return nullptr;
    }
    auto type = expr->getType();
    if (!type) {
        logger_->error(start, "undefined type in unary expression.");
        return nullptr;
    }
    if (expr->isConstant()) {
        return fold(start, end, op, expr.get());
    }
    return make_unique<UnaryExpressionNode>(start, op, std::move(expr), type);
}

unique_ptr<ExpressionNode>
Sema::onBinaryExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                         OperatorType op, unique_ptr<ExpressionNode> lhs, unique_ptr<ExpressionNode> rhs) {
    if (!lhs) {
        logger_->error(start, "undefined left-hand side in binary expression.");
        return nullptr;
    }
    if (!rhs) {
        logger_->error(start, "undefined right-hand side in binary expression.");
        return nullptr;
    }
    auto lhsType = lhs->getType();
    if (!lhsType) {
        logger_->error(lhs->pos(), "undefined left-hand side type in binary expression.");
        return nullptr;
    }
    auto rhsType = rhs->getType();
    if (!rhsType) {
        logger_->error(lhs->pos(), "undefined right-hand side type in binary expression.");
        return nullptr;
    }
    // Type inference
    auto common = commonType(lhsType, rhsType);
    if (!common) {
        logger_->error(start, "incompatible types (" + lhsType->getIdentifier()->name() + ", " +
                              rhsType->getIdentifier()->name() + ")");
        return nullptr;
    }
    if (op == OperatorType::EQ
        || op == OperatorType::NEQ
        || op == OperatorType::LT
        || op == OperatorType::LEQ
        || op == OperatorType::GT
        || op == OperatorType::GEQ) {
        common = this->tBoolean_;
    } else if (op == OperatorType::DIV && (!lhsType->isInteger() || !rhsType->isInteger())) {
        logger_->error(start, "integer division needs integer arguments.");
        return nullptr;
    } else if (op == OperatorType::DIVIDE) {
        if (common->isInteger()) {
            if (common->kind() == TypeKind::LONGINT) {
                common = this->tLongReal_;
            } else {
                common = this->tReal_;
            }
        }
    }
    if (lhs->isConstant() && rhs->isConstant()) {
        return fold(start, end, op, lhs.get(), rhs.get(), common);
    }
    // Casting
    if (lhs->getType() != common) {
        lhs->setCast(common);
    }
    if (rhs->getType() != common) {
        lhs->setCast(common);
    }
    return make_unique<BinaryExpressionNode>(start, op, std::move(lhs), std::move(rhs), common);
}

std::unique_ptr<ValueReferenceNode>
Sema::onValueReference(const FilePos &start, [[maybe_unused]] const FilePos &end, unique_ptr<Designator> designator) {
    auto node = make_unique<ValueReferenceNode>(start, std::move(designator));
    auto ident = node->ident();
    auto sym = symbols_->lookup(ident);
    if (!sym && ident->isQualified()) {
        // addresses the fact that 'ident.ident' is ambiguous: 'qual.ident' vs. 'ident.field'.
        auto qual = dynamic_cast<QualIdent *>(ident);
        sym = symbols_->lookup(qual->qualifier());
        if (sym) {
            node->disqualify();
        }
    }
    if (sym) {
        if (sym->getNodeType() == NodeType::constant ||
            sym->getNodeType() == NodeType::parameter ||
            sym->getNodeType() == NodeType::variable ||
            sym->getNodeType() == NodeType::procedure) {
            auto decl = dynamic_cast<DeclarationNode *>(sym);
            node->resolve(decl);
            if (node->getNodeType() == NodeType::procedure_call) {
                call(node.get());
            }
            auto type = decl->getType();
            if (!type) {
                if (sym->getNodeType() == NodeType::procedure) {
                    logger_->error(node->pos(), "function expected, found procedure.");
                    return node;   // TODO return nullptr?
                }
                logger_->error(node->pos(), "type undefined for " + to_string(*node->ident()) + ".");
                return node;   // TODO return nullptr?
            }
            size_t pos = 0;
            size_t last = node->getSelectorCount();
            while (pos < last) {
                auto sel = node->getSelector(pos);
                if (type->getNodeType() != sel->getType()) {
                    if (type->getNodeType() == NodeType::pointer_type &&
                        (sel->getType() == NodeType::array_type || sel->getType() == NodeType::record_type)) {
                        // perform implicit pointer dereferencing
                        auto caret = std::make_unique<Dereference>(EMPTY_POS);
                        sel = caret.get();
                        node->insertSelector(pos, std::move(caret));
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
                            logger_->error(node->pos(), "undefined identifier: " + to_string(*guard->ident()) + ".");
                        }
                    } else {
                        logger_->error(sel->pos(), "selector type mismatch.");
                    }
                }
                if (sel->getType() == NodeType::array_type && type->isArray()) {
                    auto array_t = dynamic_cast<ArrayTypeNode *>(type);
                    auto expr = dynamic_cast<ArrayIndex*>(sel)->getExpression();
                    // expr->accept(*this);
                    auto sel_type = expr->getType();
                    if (sel_type && sel_type->kind() != TypeKind::INTEGER) {
                        logger_->error(sel->pos(), "integer expression expected.");
                    }
                    if (!expr->isConstant() || !expr->isLiteral()) {
                        logger_->error(expr->pos(), "constant expression expected.");
                        // auto value = fold(expr);
                        // if (value) {
                        //     node.setSelector(pos, std::make_unique<ArrayIndex>(EMPTY_POS, std::move(value)));
                        // }
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
            node->setType(resolveType(type));
        } else {
            logger_->error(node->pos(), "constant, parameter, variable, function call expected.");
        }
    } else {
        logger_->error(node->pos(), "undefined identifier: " + to_string(*node->ident()) + ".");
    }
    return node;
}

unique_ptr<BooleanLiteralNode>
Sema::onBooleanLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, bool value) {
    return make_unique<BooleanLiteralNode>(start, value, this->tBoolean_);
}

unique_ptr<IntegerLiteralNode>
Sema::onIntegerLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, long value, bool ext) {
    TypeNode *type = ext ? this->tLongInt_ : this->tInteger_;
    return make_unique<IntegerLiteralNode>(start, value, type);
}

unique_ptr<RealLiteralNode>
Sema::onRealLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, double value, bool ext) {
    TypeNode *type = ext ? this->tLongReal_ : this->tReal_;
    return make_unique<RealLiteralNode>(start, value, type);
}

unique_ptr<StringLiteralNode>
Sema::onStringLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, const string &value) {
    return make_unique<StringLiteralNode>(start, value, this->tString_);
}

unique_ptr<NilLiteralNode>
Sema::onNilLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end) {
    return make_unique<NilLiteralNode>(start, symbols_->getNilType());
}

bool
Sema::foldBoolean(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::boolean) {
        return dynamic_cast<const BooleanLiteralNode *>(expr)->value();
    }
    logger_->error(start, "expression is not a constant boolean value.");
    return false;
}

long
Sema::foldInteger(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::integer) {
        return dynamic_cast<const IntegerLiteralNode *>(expr)->value();
    }
    logger_->error(start, "expression is not a constant integer value.");
    return 0;
}

double
Sema::foldReal(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::real) {
        return dynamic_cast<const RealLiteralNode *>(expr)->value();
    } else if (expr->getNodeType() == NodeType::integer) {
        // promote integer to real
        return (double) foldInteger(start, end, expr);
    }
    logger_->error(start, "expression is not a constant real value.");
    return 0.0;
}

string
Sema::foldString(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::string) {
        return dynamic_cast<const StringLiteralNode *>(expr)->value();
    }
    logger_->error(start, "expression is not a constant real value.");
    return "";
}

unique_ptr<LiteralNode>
Sema::fold(const FilePos &start, [[maybe_unused]] const FilePos &end, OperatorType op, ExpressionNode *expr) {
    auto type = expr->getType();
    auto cast = expr->getCast();
    if (type->isBoolean()) {
        bool value = foldBoolean(start, end, expr);
        switch (op) {
            case OperatorType::NOT:
                return make_unique<BooleanLiteralNode>(start, !value, type, cast);
            default:
                logger_->error(start, "operator " + to_string(op) + " cannot be applied to value values.");
        }
    } else if (type->isInteger()) {
        long value = foldInteger(start, end, expr);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<IntegerLiteralNode>(start, value, type, cast);
            case OperatorType::NEG:
                return make_unique<IntegerLiteralNode>(start, -value, type, cast);
            default:
                logger_->error(start, "operator " + to_string(op) + " cannot be applied to value values.");
        }
    } else if (type->isReal()) {
        double value = foldReal(start, end, expr);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<RealLiteralNode>(start, value, type, cast);
            case OperatorType::NEG:
                return make_unique<RealLiteralNode>(start, -value, type, cast);
            default:
                logger_->error(start, "operator " + to_string(op) + " cannot be applied to value values.");
        }
    }
    logger_->error(start, "invalid unary expression.");
    return nullptr;
}

unique_ptr<LiteralNode>
Sema::fold(const FilePos &start, [[maybe_unused]] const FilePos &end,
           OperatorType op, ExpressionNode *lhs, ExpressionNode *rhs, TypeNode* common) {
    if (common->isBoolean()) {
        if (lhs->getType()->isBoolean() && rhs->getType()->isBoolean()) {
            bool lvalue = foldBoolean(start, end, lhs);
            bool rvalue = foldBoolean(start, end, rhs);
            switch (op) {
                case OperatorType::AND:
                    return make_unique<BooleanLiteralNode>(start, lvalue && rvalue, common);
                case OperatorType::OR:
                    return make_unique<BooleanLiteralNode>(start, lvalue || rvalue, common);
                default:
                    logger_->error(start, "operator " + to_string(op) + " cannot be applied to boolean values.");
            }
        } else if (lhs->getType()->isNumeric() && rhs->getType()->isNumeric()) {
            if (lhs->getType()->isInteger() && rhs->getType()->isInteger()) {
                long lvalue = foldInteger(start, end, lhs);
                long rvalue = foldInteger(start, end, rhs);
                switch (op) {
                    case OperatorType::EQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                    case OperatorType::NEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
                    case OperatorType::LT:
                        return make_unique<BooleanLiteralNode>(start, lvalue <= rvalue, common);
                    case OperatorType::LEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue < rvalue, common);
                    case OperatorType::GT:
                        return make_unique<BooleanLiteralNode>(start, lvalue > rvalue, common);
                    case OperatorType::GEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue >= rvalue, common);
                    default:
                        logger_->error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
                }
            } else {
                double lvalue = foldReal(start, end, lhs);
                double rvalue = foldReal(start, end, rhs);
                switch (op) {
                    case OperatorType::EQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                    case OperatorType::NEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
                    case OperatorType::LT:
                        return make_unique<BooleanLiteralNode>(start, lvalue <= rvalue, common);
                    case OperatorType::LEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue < rvalue, common);
                    case OperatorType::GT:
                        return make_unique<BooleanLiteralNode>(start, lvalue > rvalue, common);
                    case OperatorType::GEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue >= rvalue, common);
                    default:
                        logger_->error(start, "operator " + to_string(op) + " cannot be applied to real values.");
                }
            }
        } else if (lhs->getType()->isString() && rhs->getType()->isString()) {
            string lvalue = foldString(start, end, lhs);
            string rvalue = foldString(start, end, rhs);
            switch (op) {
                case OperatorType::EQ:
                    return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                case OperatorType::NEQ:
                    return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
                default:
                    logger_->error(start, "operator " + to_string(op) + " cannot be applied to string values.");
            }
        } else if (lhs->getType()->kind() == TypeKind::NILTYPE && rhs->getType()->kind() == TypeKind::NILTYPE) {
            switch (op) {
                case OperatorType::EQ:
                    return make_unique<BooleanLiteralNode>(start, true, common);
                case OperatorType::NEQ:
                    return make_unique<BooleanLiteralNode>(start, false, common);
                default:
                    logger_->error(start, "operator " + to_string(op) + " cannot be applied to NIL values.");
            }
        }
    } else if (common->isInteger()) {
        long lvalue = foldInteger(start, end, lhs);
        long rvalue = foldInteger(start, end, rhs);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<IntegerLiteralNode>(start, lvalue + rvalue, common);
            case OperatorType::MINUS:
                return make_unique<IntegerLiteralNode>(start, lvalue - rvalue, common);
            case OperatorType::TIMES:
                return make_unique<IntegerLiteralNode>(start, lvalue * rvalue, common);
            case OperatorType::DIV:
                return make_unique<IntegerLiteralNode>(start, lvalue / rvalue, common);
            case OperatorType::MOD:
                return make_unique<IntegerLiteralNode>(start, lvalue % rvalue, common);
            default:
                logger_->error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
        }
    } else if (common->isReal()) {
        double lvalue = foldReal(start, end, lhs);
        double rvalue = foldReal(start, end, rhs);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<IntegerLiteralNode>(start, lvalue + rvalue, common);
            case OperatorType::MINUS:
                return make_unique<IntegerLiteralNode>(start, lvalue - rvalue, common);
            case OperatorType::TIMES:
                return make_unique<IntegerLiteralNode>(start, lvalue * rvalue, common);
            case OperatorType::DIVIDE:
                return make_unique<IntegerLiteralNode>(start, lvalue / rvalue, common);
            default:
                logger_->error(start, "operator " + to_string(op) + " cannot be applied to real values.");
        }
    } else if (common->isString()) {
        string lvalue = foldString(start, end, lhs);
        string rvalue = foldString(start, end, rhs);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<StringLiteralNode>(start, lvalue + rvalue, common);
            default:
                logger_->error(start, "operator " + to_string(op) + " cannot be applied to string values.");
        }
    }
    logger_->error(start, "invalid binary expression.");
    return nullptr;
}

bool
Sema::assertEqual(Ident *aIdent, Ident *bIdent) const {
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

void
Sema::assertUnique(Ident *ident, Node *node) {
    if (ident->isQualified()) {
        logger_->error(ident->start(), "cannot use qualified identifier here.");
    }
    if (symbols_->isDuplicate(ident->name())) {
        logger_->error(ident->start(), "duplicate definition: " + ident->name() + ".");
    }
    if (symbols_->isGlobal(ident->name())) {
        logger_->error(ident->start(), "predefined identifier: " + ident->name() + ".");
    }
    symbols_->insert(ident->name(), node);
}

void
Sema::checkExport(DeclarationNode *node) {
    if (node->getLevel() != SymbolTable::MODULE_LEVEL && node->getIdentifier()->isExported()) {
        logger_->error(node->getIdentifier()->start(), "only top-level declarations can be exported.");
    }
}

bool
Sema::assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual, bool isPtr) {
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

TypeNode *
Sema::commonType(TypeNode *lhsType, TypeNode *rhsType) const {
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

string
Sema::format(const TypeNode *type, bool isPtr) const {
    auto name = isPtr ? string("POINTER TO ") : string();
    if (type->getIdentifier()) {
        name += to_string(*type->getIdentifier());
    } else {
        std::stringstream stream;
        stream << *type;
        name += stream.str();
    }
    return name;
}

void
Sema::call(ProcedureNodeReference *node) {
    auto proc = dynamic_cast<ProcedureNode *>(node->dereference());
    if (node->ident()->isQualified() && proc->isExtern()) {
        // a fully-qualified external reference needs to be added to module for code generation
        context_->addExternalProcedure(proc);
    }
    if (node->getActualParameterCount() < proc->getFormalParameterCount()) {
        logger_->error(node->pos(), "fewer actual than formal parameters.");
    }
    for (size_t cnt = 0; cnt < node->getActualParameterCount(); cnt++) {
        auto expr = node->getActualParameter(cnt);
        // expr->accept(*this);
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
        // if (expr->isConstant()) {
        //    node->setActualParameter(cnt, fold(expr));
        // }
    }
}

TypeNode *
Sema::resolveType(TypeNode *type) {
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
