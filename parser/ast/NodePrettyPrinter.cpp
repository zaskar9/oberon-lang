/*
 * Header of the node visitor that pretty-prints the abstract syntax tree used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/31/2018.
 */

#include "NodePrettyPrinter.h"

NodePrettyPrinter::NodePrettyPrinter(std::ostream &stream) : indent_(0), stream_(stream) {
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

void NodePrettyPrinter::visit(ModuleNode& node) {
    indent();
    stream_ << "MODULE " << node.getName() << "{" << node.getLevel() << "};" << std::endl;
    block(node, true);
    stream_ << std::endl;
    indent();
    stream_ << "BEGIN" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "END " << node.getName() << '.' << std::endl;
}

void NodePrettyPrinter::visit(ProcedureNode& node) {
    indent();
    stream_ << "PROCEDURE " << node.getName() << "{" << node.getLevel() << "}(";
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        node.getParameter(i)->accept(*this);
        if (i + 1 < node.getParameterCount()) {
            stream_ << ", ";
        }
    }
    stream_ << ");" << std::endl;
    block(node, false);
    indent();
    stream_ << "BEGIN" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "END " << node.getName() << ';' << std::endl;
}

void NodePrettyPrinter::visit(ReferenceNode &node) {
    auto ref = node.dereference();
    stream_ << ref->getName();
    if (node.getSelector() != nullptr) {
        auto type = ref->getType();
        if (type->getNodeType() == NodeType::record_type) {
            stream_ << ".";
            node.getSelector()->accept(*this);
        } else if (type->getNodeType() == NodeType::array_type) {
            stream_ << "[";
            node.getSelector()->accept(*this);
            stream_ << "]";
        }
    }
}

void NodePrettyPrinter::visit(ConstantDeclarationNode &node) {
    stream_ << node.getName() << "{" << node.getLevel() << "} = ";
    node.getValue()->accept(*this);
    stream_ << ';' << std::endl;
}

void NodePrettyPrinter::visit(FieldNode &node) {
    stream_ << node.getName() << "{" << node.getOffset() << "}" << ": ";
    node.getType()->accept(*this);
    stream_ << ';';
}

void NodePrettyPrinter::visit(ParameterNode &node) {
    stream_ << (node.isVar() ? "VAR " : "");
    stream_ << node.getName() << "{" << node.getLevel() << "}: ";
    node.getType()->accept(*this);
}

void NodePrettyPrinter::visit(TypeDeclarationNode &node) {
    stream_ << node.getName() << "{" << node.getLevel() << "} = ";
    node.getType()->accept(*this);
    stream_ << ';' << std::endl;
}

void NodePrettyPrinter::visit(VariableDeclarationNode &node) {
    stream_ << node.getName() << "{" << node.getLevel() << "@" << node.getOffset() << "}: ";
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
    stream_ << node.getValue();
}

void NodePrettyPrinter::visit(UnaryExpressionNode &node) {
    stream_ << node.getOperator();
    node.getExpression()->accept(*this);
}

void NodePrettyPrinter::visit(BinaryExpressionNode &node) {
    node.getLeftExpression()->accept(*this);
    stream_ << ' ' << node.getOperator() << ' ';
    node.getRightExpression()->accept(*this);
}

void NodePrettyPrinter::visit(ArrayTypeNode &node) {
    stream_ << "ARRAY " << node.getDimension() << " OF ";
    node.getMemberType()->accept(*this);
}

void NodePrettyPrinter::visit(BasicTypeNode &node) {
    stream_ << node.getName();
}

void NodePrettyPrinter::visit(RecordTypeNode &node) {
    stream_ << "RECORD ";
    for (size_t i = 0; i < node.getFieldCount(); i++) {
        node.getField(i)->accept(*this);
        stream_ << ' ';
    }
    stream_ << "END";
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
    stream_ << node.getProcedure()->getName() << "(";
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        node.getParameter(i)->accept(*this);
        if (i + 1 < node.getParameterCount()) {
            stream_ << ", ";
        }
    }
    stream_ << ")";
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
    stream_ << " BY " << node.getStep() << " DO" << std::endl;
    indent_ += TAB_WIDTH;
    node.getStatements()->accept(*this);
    indent_ -= TAB_WIDTH;
    indent();
    stream_ << "END";
}
