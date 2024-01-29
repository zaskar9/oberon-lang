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

ArrayTypeNode *Sema::onArrayType(const FilePos &start, [[maybe_unused]] const FilePos &end,
                                 unique_ptr<ExpressionNode> expr, TypeNode *type) {
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
        auto res = context_->getOrInsertArrayType((unsigned int) dim->value(), mem_t);
        if (mem_t->getSize() > 0 && dim->value() > 0) {
            res->setSize(dim->value() * mem_t->getSize());
        } else {
            logger_->error(start, "undefined array dimension.");
        }
        return res;
    }
    return nullptr;
}

TypeNode *Sema::onTypeReference([[maybe_unused]] const FilePos &start, [[maybe_unused]] const FilePos &end, unique_ptr<QualIdent> ident) {
    auto type = symbols_->lookup(ident.get());
    if (type) {
        return dynamic_cast<TypeNode*>(type);   // TODO Yolo!
    }
    // TODO
    return nullptr;
}

unique_ptr<ExpressionNode>
Sema::onUnaryExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                        OperatorType op, unique_ptr<ExpressionNode> expr) {
    if (!expr) {
        logger_->error(start, "undefined expression.");
        return nullptr;
    }
    auto type = expr->getType();
    if (!type) {
        logger_->error(start, "undefined expression type.");
        return nullptr;
    }
    if (expr->isConstant()) {
        auto cast = expr->getCast();
        if (expr->getNodeType() == NodeType::integer) {
            auto integer = dynamic_cast<const IntegerLiteralNode*>(expr.get());
            switch (op) {
                case OperatorType::PLUS: return make_unique<IntegerLiteralNode>(start, integer->value(), type, cast);
                case OperatorType::NEG: return make_unique<IntegerLiteralNode>(start, -integer->value(), type, cast);
                default:
                    logger_->error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
            }
        } else if (expr->getNodeType() == NodeType::real) {
            auto real = dynamic_cast<const RealLiteralNode*>(expr.get());
            switch (op) {
                case OperatorType::PLUS: return make_unique<RealLiteralNode>(start, real->value(), type, cast);
                case OperatorType::NEG: return make_unique<RealLiteralNode>(start, -real->value(), type, cast);
                default:
                    logger_->error(start, "operator " + to_string(op) + " cannot be applied to real values.");
            }
        } else if (expr->getNodeType() == NodeType::boolean) {
            auto boolean = dynamic_cast<const BooleanLiteralNode*>(expr.get());
            switch (op) {
                case OperatorType::NOT: return make_unique<RealLiteralNode>(start, !boolean->value(), type, cast);
                default:
                    logger_->error(start, "operator " + to_string(op) + " cannot be applied to real values.");
            }
        }
    }
    return make_unique<UnaryExpressionNode>(start, op, std::move(expr));
}

unique_ptr<BooleanLiteralNode> Sema::onBooleanLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, bool value) {
    return make_unique<BooleanLiteralNode>(start, value, this->tBoolean_);
}

unique_ptr<IntegerLiteralNode> Sema::onIntegerLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, long value, bool ext) {
    TypeNode *type = ext ? this->tLongInt_ : this->tInteger_;
    return make_unique<IntegerLiteralNode>(start, value, type);
}

unique_ptr<RealLiteralNode> Sema::onRealLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, double value, bool ext) {
    TypeNode *type = ext ? this->tLongReal_ : this->tReal_;
    return make_unique<RealLiteralNode>(start, value, type);
}

unique_ptr<StringLiteralNode> Sema::onStringLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, const string &value) {
    return make_unique<StringLiteralNode>(start, value, this->tString_);
}

unique_ptr<NilLiteralNode> Sema::onNilLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end) {
    return make_unique<NilLiteralNode>(start, symbols_->getNilType());
}

TypeNode *Sema::resolveType(TypeNode *type) {
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
