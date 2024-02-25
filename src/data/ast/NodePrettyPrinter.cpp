/*
 * Pretty printer for all nodes of the AST used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/31/2018.
 */

#include "NodePrettyPrinter.h"

#include <memory>
#include <string>
#include <vector>

#include "scanner/Scanner.h"

using std::string;
using std::unique_ptr;
using std::vector;

void NodePrettyPrinter::print(Node *node) {
    node->accept(*this);
}

void NodePrettyPrinter::indent() {
    stream_ << string(indent_, ' ');
}

void NodePrettyPrinter::block(BlockNode& node, bool isGlobal) {
    indent_ += TAB_WIDTH;
    if (node.getConstantCount() > 0) {
        if (isGlobal) {
            stream_ << std::endl;
        }
        indent();
        stream_ << "CONST ";
        node.getConstant(0)->accept(*this);
        indent_ += 6;
        for (size_t i = 1; i < node.getConstantCount(); i++) {
            indent();
            node.getConstant(i)->accept(*this);
        }
        indent_ -= 6;
    }
    if (node.getTypeDeclarationCount() > 0) {
        if (isGlobal) {
            stream_ << std::endl;
        }
        indent();
        stream_ << "TYPE ";
        node.getTypeDeclaration(0)->accept(*this);
        indent_ += 5;
        for (size_t i = 1; i < node.getTypeDeclarationCount(); i++) {
            indent();
            node.getTypeDeclaration(i)->accept(*this);
        }
        indent_ -= 5;
    }
    if (node.getVariableCount() > 0) {
        if (isGlobal) {
            stream_ << std::endl;
        }
        indent();
        stream_ << "VAR ";
        node.getVariable(0)->accept(*this);
        indent_ += 4;
        for (size_t i = 1; i < node.getVariableCount(); i++) {
            indent();
            node.getVariable(i)->accept(*this);
        }
        indent_ -= 4;
    }
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        stream_ << std::endl;
        node.getProcedure(i)->accept(*this);
    }
    indent_ -= TAB_WIDTH;
}

void NodePrettyPrinter::visit(ModuleNode& node) {
    indent();
    stream_ << "MODULE " << *node.getIdentifier() << "(*" << node.getLevel() << "*);" << std::endl;
    if (node.getImportCount() > 0) {
        stream_ << "IMPORT ";
        for (size_t i = 0; i < node.getImportCount(); i++) {
            node.getImport(i)->accept(*this);
        }
        stream_ << std::endl;
    }
    block(node, true);
    stream_ << std::endl;
    if (node.statements()->getStatementCount() > 0) {
        indent();
        stream_ << "BEGIN" << std::endl;
        indent_ += TAB_WIDTH;
        node.statements()->accept(*this);
        indent_ -= TAB_WIDTH;
    }
    indent();
    stream_ << "END " << *node.getIdentifier() << '.' << std::endl;
}

void NodePrettyPrinter::visit(ProcedureNode& node) {
    indent();
    stream_ << "PROCEDURE " << *node.getIdentifier() << "(*" << node.getLevel() << "*)(";
    auto type = dynamic_cast<ProcedureTypeNode *>(node.getType());
    string sep;
    for (auto &param : type->parameters()) {
        stream_ << sep;
        param->accept(*this);
        sep = "; ";
    }
    if (type->hasVarArgs()) {
        stream_ << "; ...";
    }
    stream_ << ")";
    if (type->getReturnType() != nullptr) {
        stream_ << ": ";
        type->getReturnType()->accept(*this);
    }
    stream_ << ";";
    if (node.isExtern()) {
        stream_ << " EXTERN;" << std::endl;
    } else {
        stream_ << std::endl;
        block(node, false);
        if (node.statements()->getStatementCount() > 0) {
            indent();
            stream_ << "BEGIN" << std::endl;
            indent_ += TAB_WIDTH;
            node.statements()->accept(*this);
            indent_ -= TAB_WIDTH;
        }
        indent();
        stream_ << "END " << *node.getIdentifier() << ';' << std::endl;
    }
}

void NodePrettyPrinter::visit(ImportNode &node) {
    if (node.getAlias()) {
        stream_ << *node.getAlias() << " := ";
    }
    stream_ << *node.getModule() << "; ";
}

void NodePrettyPrinter::visit(QualifiedStatement &node) {
    stream_ << *node.ident();
    selectors(node.selectors());
}

void NodePrettyPrinter::visit(QualifiedExpression &node) {
    stream_ << *node.ident();
    selectors(node.selectors());
}

void NodePrettyPrinter::selectors(std::vector<unique_ptr<Selector>> &selectors) {
    for (auto &sel: selectors) {
        auto selector = sel.get();
        auto type = selector->getNodeType();
        if (type == NodeType::parameter) {
            auto params = dynamic_cast<ActualParameters *>(selector);
            stream_ << "(";
            string sep;
            for (auto &param : params->parameters()) {
                stream_ << sep;
                param->accept(*this);
                sep = ", ";
            }
            stream_ << ")";
        } else if (type == NodeType::array_type) {
            auto indices = dynamic_cast<ArrayIndex *>(selector);
            stream_ << "[";
            string sep;
            for (auto &index : indices->indices()) {
                stream_ << sep;
                index->accept(*this);
                sep = ", ";
            }
            stream_ << "]";
        } else if (type == NodeType::record_type) {
            stream_ << ".";
            stream_ << *dynamic_cast<RecordField *>(selector)->getField()->getIdentifier();
        } else if (type == NodeType::pointer_type) {
            stream_ << "^";
        } else if (type == NodeType::type) {
            stream_ << "(";
            stream_ << *dynamic_cast<Typeguard *>(selector)->ident();
            stream_ << ")";
        }
    }
}

void NodePrettyPrinter::visit(ConstantDeclarationNode &node) {
    stream_ << *node.getIdentifier() << "(*" << node.getLevel() << "*) = ";
    node.getValue()->accept(*this);
    stream_ << "(*" << *node.getType()->getIdentifier() << "*);" << std::endl;
}

void NodePrettyPrinter::visit(FieldNode &node) {
    stream_ << *node.getIdentifier() << ": ";
    node.getType()->accept(*this);
}

void NodePrettyPrinter::visit(ParameterNode &node) {
    stream_ << (node.isVar() ? "VAR " : "");
    stream_ << *node.getIdentifier() << "(*" << node.getLevel() << "*): ";
    node.getType()->accept(*this);
}

void NodePrettyPrinter::visit(TypeDeclarationNode &node) {
    stream_ << *node.getIdentifier() << "(*" << node.getLevel() << "*) = ";
    isDecl_ = true;
    node.getType()->accept(*this);
    stream_ << ';' << std::endl;
}

void NodePrettyPrinter::visit(VariableDeclarationNode &node) {
    stream_ << *node.getIdentifier() << "(*" << node.getLevel() << "*): ";
    node.getType()->accept(*this);
    stream_ << ';' << std::endl;
}

void NodePrettyPrinter::visit(BooleanLiteralNode &node) {
    stream_ << (node.value() ? "TRUE" : "FALSE");
}

void NodePrettyPrinter::visit(IntegerLiteralNode &node) {
    stream_ << node.value();
}

void NodePrettyPrinter::visit(RealLiteralNode &node) {
    stream_ << node.value();
}

void NodePrettyPrinter::visit(StringLiteralNode &node) {
    stream_ << "\"" << Scanner::escape(node.value()) << "\"";
}

void NodePrettyPrinter::visit(NilLiteralNode &) {
    stream_ << "NIL";
}

void NodePrettyPrinter::visit(SetLiteralNode &node) {
    if (node.value().none()) {
        stream_ << "{}";
    } else {
        auto set = node.value();
        string sep;
        stream_ << "{ ";
        for (std::size_t bit = 0; bit < set.size(); ++bit) {
            if (set.test(bit)) {
                stream_ << sep << bit;
                sep = ", ";
            }
        }
        stream_ << " } (* " << set.to_ulong() << ": " << set << " *)";
    }
}

void NodePrettyPrinter::visit(RangeLiteralNode &node) {
    auto set = node.value();
    string sep;
    for (std::size_t bit = 0; bit < set.size(); ++bit) {
        if (set.test(bit)) {
            stream_ << sep << bit;
            sep = ", ";
        }
    }
}

void NodePrettyPrinter::visit(UnaryExpressionNode &node) {
    stream_ << node.getOperator();
    auto expr = node.getExpression();
    stream_ << (expr->getPrecedence() < node.getPrecedence() ? "(" : "");
    node.getExpression()->accept(*this);
    stream_ << (expr->getPrecedence() < node.getPrecedence() ? ")" : "");
}

void NodePrettyPrinter::visit(BinaryExpressionNode &node) {
    auto lhs = node.getLeftExpression();
    stream_ << (lhs->getPrecedence() < node.getPrecedence() ? "(" : "");
    lhs->accept(*this);
    stream_ << (lhs->getPrecedence() < node.getPrecedence() ? ")" : "");
    stream_ << ' ' << node.getOperator() << ' ';
    auto rhs = node.getRightExpression();
    stream_ << (rhs->getPrecedence() < node.getPrecedence() ? "(" : "");
    node.getRightExpression()->accept(*this);
    stream_ << (rhs->getPrecedence() < node.getPrecedence() ? ")" : "");
}

void NodePrettyPrinter::visit(RangeExpressionNode &node) {
    node.getLower()->accept(*this);
    stream_ << " .. ";
    node.getUpper()->accept(*this);
}

void NodePrettyPrinter::visit(SetExpressionNode &node) {
    if (node.isEmptySet()) {
        stream_ << "{}";
    } else {
        stream_ << "{ ";
        string sep;
        for (auto &element : node.elements()) {
            stream_ << sep;
            element->accept(*this);
            sep = ", ";
        }
        stream_ << " }";
    }
}

void NodePrettyPrinter::visit(ArrayTypeNode &node) {
    if (node.isAnonymous() || isDecl_) {
        isDecl_ = false;
        stream_ << "ARRAY " << node.getDimension() << " OF ";
        node.getMemberType()->accept(*this);
    } else {
        stream_ << *node.getIdentifier();
    }
}

void NodePrettyPrinter::visit(BasicTypeNode &node) {
    stream_ << *node.getIdentifier();
}

void NodePrettyPrinter::visit([[maybe_unused]] ProcedureTypeNode &node) {
    // For the time being, handled by visit(ProcedureNode).
}

void NodePrettyPrinter::visit(RecordTypeNode &node) {
    if (node.isAnonymous() || isDecl_) {
        isDecl_ = false;
        stream_ << "RECORD ";
        for (size_t i = 0; i < node.getFieldCount(); i++) {
            node.getField(i)->accept(*this);
            if (i + 1 < node.getFieldCount()) {
                stream_ << "; ";
            } else {
                stream_ << ' ';
            }
        }
        stream_ << "END";
    } else {
        stream_ << *node.getIdentifier();
    }
}

void NodePrettyPrinter::visit(PointerTypeNode &node) {
    if (node.isAnonymous() || isDecl_) {
        isDecl_ = false;
        stream_ << "POINTER TO ";
        node.getBase()->accept(*this);
    } else {
        stream_ << *node.getIdentifier();
    }
}

void NodePrettyPrinter::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        indent();
        node.getStatement(i)->accept(*this);
        if (i + 1 < node.getStatementCount()) {
            stream_ << ";";
        }
        stream_ << std::endl;
    }
}

void NodePrettyPrinter::visit(AssignmentNode &node) {
    node.getLvalue()->accept(*this);
    stream_ << " := ";
    node.getRvalue()->accept(*this);
}

void NodePrettyPrinter::visit(IfThenElseNode &node) {
    stream_ << "IF ";
    node.getCondition()->accept(*this);
    stream_ << " THEN" << std::endl;
    indent_ += TAB_WIDTH;
    node.getThenStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    for (size_t i = 0; i < node.getElseIfCount(); i++) {
        indent();
        node.getElseIf(i)->accept(*this);
    }
    if (node.hasElse()) {
        indent();
        stream_ << "ELSE" << std::endl;
        indent_ += TAB_WIDTH;
        node.getElseStatements()->accept(*this);
        indent_ -= TAB_WIDTH;
    }
    indent();
    stream_ << "END";
}

void NodePrettyPrinter::visit(ElseIfNode& node) {
    stream_ << "ELSIF ";
    node.getCondition()->accept(*this);
    stream_ << " THEN" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
}

void NodePrettyPrinter::visit(LoopNode &node) {
    stream_ << "LOOP" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "END";
}

void NodePrettyPrinter::visit(WhileLoopNode &node) {
    stream_ << "WHILE ";
    node.getCondition()->accept(*this);
    stream_ << " DO" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "END";
}

void NodePrettyPrinter::visit(RepeatLoopNode &node) {
    stream_ << "REPEAT" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "UNTIL ";
    node.getCondition()->accept(*this);
}

void NodePrettyPrinter::visit(ForLoopNode &node) {
    stream_ << "FOR ";
    node.getCounter()->accept(*this);
    stream_ << " := ";
    node.getLow()->accept(*this);
    stream_ << " TO ";
    node.getHigh()->accept(*this);
    stream_ << " BY ";
    node.getStep()->accept(*this);
    stream_ << " DO" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "END";
}

void NodePrettyPrinter::visit(ReturnNode &node) {
    stream_ << "RETURN";
    if (node.getValue()) {
        stream_ << " ";
        node.getValue()->accept(*this);
    }
}