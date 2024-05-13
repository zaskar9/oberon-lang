/*
 * Analysis pass that removes nested procedures used the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/9/20.
 */

#ifndef OBERON_LLVM_LAMBDALIFTER_H
#define OBERON_LLVM_LAMBDALIFTER_H


#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "Analyzer.h"
#include "data/ast/ASTContext.h"
#include "data/ast/NodeVisitor.h"

using std::string;
using std::unique_ptr;
using std::vector;

class LambdaLifter final : public Analysis, private NodeVisitor {

private:
    ASTContext *context_;
    ModuleNode *module_;
    DeclarationNode *env_;
    unsigned int scope_;
    vector<string> path_;

    static const string THIS_;
    static const string SUPER_;
    static const FilePos POS_;

    ParameterNode *findParameter(string, vector<unique_ptr<ParameterNode>> &);

    void declaration(DeclarationNode *, vector<unique_ptr<Selector>> &);
    void selectors(TypeNode *, vector<unique_ptr<Selector>> &);

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
    void visit(CaseNode &) override;
    void visit(IfThenElseNode &) override;
    void visit(ElseIfNode &) override;
    void visit(LoopNode &) override;
    void visit(WhileLoopNode &) override;
    void visit(RepeatLoopNode &) override;
    void visit(ForLoopNode &) override;
    void visit(ReturnNode &) override;

    static bool envFieldResolver(QualifiedExpression *, const string &, TypeNode *);

public:
    explicit LambdaLifter(ASTContext *context) : context_(context), module_(), env_(), scope_(), path_() { };
    ~LambdaLifter() override = default;

    void run(Logger &, Node *) override;

};


#endif //OBERON_LLVM_LAMBDALIFTER_H
