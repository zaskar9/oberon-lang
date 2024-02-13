/*
 * Analysis pass that removes nested procedures used the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/9/20.
 */

#ifndef OBERON_LLVM_LAMBDALIFTER_H
#define OBERON_LLVM_LAMBDALIFTER_H


#include "Analyzer.h"
#include "data/ast/ASTContext.h"
#include "data/ast/NodeVisitor.h"

class LambdaLifter final : public Analysis, private NodeVisitor {

private:
    ASTContext *context_;
    ModuleNode *module_;
    DeclarationNode *env_;
    unsigned int level_;

    static const std::string THIS_;
    static const std::string SUPER_;
    static const FilePos POS_;

    void call(ProcedureNodeReference &);

    void visit(ModuleNode &) override;
    void visit(ProcedureNode &) override;

    void visit(ImportNode &) override;

    void visit(ConstantDeclarationNode &) override;
    void visit(FieldNode &) override;
    void visit(ParameterNode &) override;
    void visit(TypeDeclarationNode &) override;
    void visit(VariableDeclarationNode &) override;

    void visit(ValueReferenceNode &) override;
    void visit(QualifiedExpression &) override;

    void visit(BooleanLiteralNode &) override;
    void visit(IntegerLiteralNode &) override;
    void visit(RealLiteralNode &) override;
    void visit(StringLiteralNode &) override;
    void visit(NilLiteralNode &) override;

    void visit(UnaryExpressionNode &) override;
    void visit(BinaryExpressionNode &) override;

    void visit(ArrayTypeNode &) override;
    void visit(BasicTypeNode &) override;
    void visit(ProcedureTypeNode &) override;
    void visit(RecordTypeNode &) override;
    void visit(PointerTypeNode &) override;

    void visit(StatementSequenceNode &) override;
    void visit(AssignmentNode &) override;
    void visit(IfThenElseNode &) override;
    void visit(ElseIfNode &) override;
    void visit(ProcedureCallNode &) override;
    void visit(LoopNode &) override;
    void visit(WhileLoopNode &) override;
    void visit(RepeatLoopNode &) override;
    void visit(ForLoopNode &) override;
    void visit(ReturnNode &) override;

    static bool envFieldResolver(ValueReferenceNode *var, const std::string &field_name, TypeNode *field_type) ;

public:
    explicit LambdaLifter(ASTContext *context) : context_(context), module_(), env_(), level_() { };
    ~LambdaLifter() override = default;

    void run(Logger &logger, Node* node) override;

};


#endif //OBERON_LLVM_LAMBDALIFTER_H
