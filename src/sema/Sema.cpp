//
// Created by Michael Grossniklaus on 12/17/23.
//

#include "Sema.h"

#include <memory>

using std::make_unique;
using std::unique_ptr;

Sema::Sema(ASTContext *context, SymbolTable *symbols, Logger *logger) :
        context_(context), symbols_(symbols), logger_(logger) {
    tBoolean_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::BOOLEAN)));
    tByte_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::BYTE)));
    tChar_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::CHAR)));
    tInteger_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::INTEGER)));
    tLongInt_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::LONGINT)));
    tReal_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::REAL)));
    tLongReal_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::LONGREAL)));
    tString_ = dynamic_cast<TypeNode *>(symbols_->lookup(to_string(TypeKind::STRING)));
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
    return context_->getOrInsertProcedureNode(ident, std::move(params), ret);
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
