/*
 * Pretty printer for all nodes of the AST used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/31/2018.
 */

#ifndef OBERON0C_NODEPRETTYPRINTER_H
#define OBERON0C_NODEPRETTYPRINTER_H


#define TAB_WIDTH 3

#include "NodeVisitor.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using std::string;
using std::vector;

class NodePrettyPrinter final : private NodeVisitor {

private:
    size_t indent_;
    std::ostream &stream_;
    bool isDecl_;
    ModuleNode *module_;
    Node *parent_;

    void indent();
    void block(BlockNode &, bool isGlobal);
    void selectors(vector<unique_ptr<Selector>> &);
    void qualident(DeclarationNode *);
    void procedure(string, ProcedureTypeNode *);

    void visit(ModuleNode &) override;
    void visit(ProcedureNode &) override;

    void visit(ImportNode &) override;

    void visit(ConstantDeclarationNode &) override;
    void visit(FieldNode &) override;
    void visit(ParameterNode &) override;
    void visit(TypeDeclarationNode &) override;
    void visit(VariableDeclarationNode &) override;

    void visit(QualifiedStatement &) override;
    void visit(QualifiedExpression &) override;

    void visit(BooleanLiteralNode &) override;
    void visit(IntegerLiteralNode &) override;
    void visit(RealLiteralNode &) override;
    void visit(StringLiteralNode &) override;
    void visit(CharLiteralNode &) override;
    void visit(NilLiteralNode &) override;
    void visit(SetLiteralNode &) override;
    void visit(RangeLiteralNode &) override;

    void visit(UnaryExpressionNode &) override;
    void visit(BinaryExpressionNode &) override;
    void visit(RangeExpressionNode &) override;
    void visit(SetExpressionNode &) override;

    void visit(ArrayTypeNode &) override;
    void visit(BasicTypeNode &) override;
    void visit(ProcedureTypeNode &) override;
    void visit(RecordTypeNode &) override;
    void visit(PointerTypeNode &) override;

    void visit(StatementSequenceNode &) override;
    void visit(AssignmentNode &) override;
    void visit(CaseOfNode &) override;
    void visit(CaseLabelNode &) override;
    void visit(CaseNode &) override;
    void visit(IfThenElseNode &) override;
    void visit(ElseIfNode &) override;
    void visit(LoopNode &) override;
    void visit(WhileLoopNode &) override;
    void visit(RepeatLoopNode &) override;
    void visit(ForLoopNode &) override;
    void visit(ReturnNode &) override;
    void visit(ExitNode &) override;

public:
    explicit NodePrettyPrinter(std::ostream &stream) :
            indent_(0), stream_(stream), isDecl_(false), module_(), parent_() { };
    ~NodePrettyPrinter() override = default;

    void print(Node *node);

};


#endif //OBERON0C_NODEPRETTYPRINTER_H
