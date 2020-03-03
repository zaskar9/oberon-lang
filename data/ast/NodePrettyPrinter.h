/*
 * Pretty printer for all nodes of the AST used by the Oberon LLVM compiler.
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
    bool isDecl_;

    void indent();
    void block(BlockNode &node, bool isGlobal);
    void call(CallNode &node);

public:
    explicit NodePrettyPrinter(std::ostream &stream) : indent_(0), stream_(stream), isDecl_(false) { };
    ~NodePrettyPrinter() = default;

    void visit(ModuleNode &node) override;
    void visit(ProcedureNode &node) override;

    void visit(ReferenceNode &node) override;
    void visit(ConstantDeclarationNode &node) override;
    void visit(FieldNode &node) override;
    void visit(ParameterNode &node) override;
    void visit(VariableDeclarationNode &node) override;

    void visit(BooleanLiteralNode &node) override;
    void visit(IntegerLiteralNode &node) override;
    void visit(StringLiteralNode &node) override;
    void visit(FunctionCallNode &node) override;
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
    void visit(LoopNode &node) override;
    void visit(WhileLoopNode &node) override;
    void visit(RepeatLoopNode &node) override;
    void visit(ForLoopNode &node) override;
    void visit(ReturnNode &node) override;

};


#endif //OBERON0C_NODEPRETTYPRINTER_H