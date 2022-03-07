/*
 * Pretty printer for all nodes of the AST used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/31/2018.
 */

#include "NodePrettyPrinter.h"
#include "../../scanner/Scanner.h"

void NodePrettyPrinter::print(Node *node) {
    node->accept(*this);
}

void NodePrettyPrinter::indent() {
    stream_ << std::string(indent_, ' ');
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

void NodePrettyPrinter::call(ProcedureNodeReference &node) {
    stream_ << *node.getIdentifier() << "(";
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        node.getParameter(i)->accept(*this);
        if (i + 1 < node.getParameterCount()) {
            stream_ << ", ";
        }
    }
    stream_ << ")";
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
    indent();
    stream_ << "BEGIN" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "END " << *node.getIdentifier() << '.' << std::endl;
}

void NodePrettyPrinter::visit(ProcedureNode& node) {
    indent();
    stream_ << "PROCEDURE " << *node.getIdentifier() << "(*" << node.getLevel() << "*)(";
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        node.getParameter(i)->accept(*this);
        if (i + 1 < node.getParameterCount()) {
            stream_ << "; ";
        }
    }
    if (node.hasVarArgs()) {
        stream_ << "; ...";
    }
    stream_ << ")";
    if (node.getReturnType() != nullptr) {
        stream_ << ": ";
        node.getReturnType()->accept(*this);
    }
    stream_ << ";";
    if (node.isExtern()) {
        stream_ << " EXTERN;" << std::endl;
    } else {
        stream_ << std::endl;
        block(node, false);
        indent();
        stream_ << "BEGIN" << std::endl;
        indent_ += TAB_WIDTH;
        node.getStatements()->accept(*this);
        indent_ -= TAB_WIDTH;
        indent();
        stream_ << "END " << *node.getIdentifier() << ';' << std::endl;
    }
}

void NodePrettyPrinter::visit(ImportNode &node) {
    if (node.getAlias()) {
        stream_ << *node.getAlias() << " := ";
    }
    stream_ << *node.getModule()->getIdentifier() << "; ";
}

void NodePrettyPrinter::visit(ValueReferenceNode &node) {
    stream_ << *node.getIdentifier();
    for (size_t i = 0; i < node.getSelectorCount(); i++) {
        auto type = node.getSelectorType(i);
        if (type == NodeType::array_type) {
            stream_ << "[";
            auto selector = node.getSelector(i);
            selector->accept(*this);
            stream_ << "]";
        } else if (type == NodeType::record_type) {
            stream_ << ".";
            auto selector = node.getSelector(i);
            selector->accept(*this);
        }
    }
    // stream_ << "(*" << node.getType()->getIdentifier() << "*)";
}

void NodePrettyPrinter::visit(TypeReferenceNode &node) {
    stream_ << "->" << *node.getIdentifier();
}

void NodePrettyPrinter::visit(ConstantDeclarationNode &node) {
    stream_ << *node.getIdentifier() << "(*" << node.getLevel() << "*) = ";
    node.getValue()->accept(*this);
    stream_ << ';' << std::endl;
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
    stream_ << (node.getValue() ? "TRUE" : "FALSE");
}

void NodePrettyPrinter::visit(IntegerLiteralNode &node) {
    stream_ << node.getValue();
}

void NodePrettyPrinter::visit(StringLiteralNode &node) {
    stream_ << "\"" << Scanner::escape(node.getValue()) << "\"";
}

void NodePrettyPrinter::visit(FunctionCallNode &node) {
    call(node);
}

void NodePrettyPrinter::visit(UnaryExpressionNode &node) {
    stream_ << node.getOperator();
    node.getExpression()->accept(*this);
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
    if (node.getElseStatements() != nullptr) {
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

void NodePrettyPrinter::visit(ProcedureCallNode &node) {
    call(node);
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
    stream_ << "RETURN ";
    node.getValue()->accept(*this);
}