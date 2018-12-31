/*
 * Header of the node visitor that pretty-prints the abstract syntax tree used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/31/2018.
 */

#ifndef OBERON0C_NODEPRETTYPRINTER_H
#define OBERON0C_NODEPRETTYPRINTER_H

#define TAB_WIDTH 3

#include <iostream>
#include <iomanip>
#include "NodeVisitor.h"

class NodePrettyPrinter final : public NodeVisitor {

private:
    size_t indent_;
    std::ostream &stream_;

    void indent();
    void block(BlockNode &node, bool isGlobal);

public:
    explicit NodePrettyPrinter(std::ostream &stream);
    ~NodePrettyPrinter() = default;

    void visit(ModuleNode &node) override;
    void visit(ProcedureNode &node) override;

    void visit(NamedValueReferenceNode &node) override;
    void visit(ConstantNode &node) override;
    void visit(FieldNode &node) override;
    void visit(ParameterNode &node) override;
    void visit(VariableNode &node) override;

    void visit(BooleanNode &node) override;
    void visit(NumberNode &node) override;
    void visit(StringNode &node) override;
    void visit(UnaryExpressionNode &node) override;
    void visit(BinaryExpressionNode &node) override;

    void visit(TypeDeclarationNode &node) override;
    void visit(ArrayTypeNode &node) override;
    void visit(BasicTypeNode &node) override;
    void visit(RecordTypeNode &node) override;

    void visit(StatementSequenceNode &node) override;
    void visit(AssignmentNode &node) override;
    void visit(IfThenElseNode &node) override;
    void visit(ElseIfNode &node) override;
    void visit(ProcedureCallNode &node) override;
    void visit(WhileLoopNode &node) override;

};


#endif //OBERON0C_NODEPRETTYPRINTER_H
