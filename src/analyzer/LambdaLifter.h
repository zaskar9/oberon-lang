/*
 * Analysis pass that removes nested procedures used the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/9/20.
 */

#ifndef OBERON_LLVM_LAMBDALIFTER_H
#define OBERON_LLVM_LAMBDALIFTER_H


#include "Analyzer.h"
#include "../data/ast/NodeVisitor.h"

class LambdaLifter final : public Analysis, private NodeVisitor {

private:
    ModuleNode *module_;
    DeclarationNode *env_;
    unsigned int level_;

    static const std::string THIS_;
    static const std::string SUPER_;
    static const FilePos POS_;

    void call(ProcedureNodeReference &node);

    void visit(ModuleNode &node) override;
    void visit(ProcedureNode &node) override;

    void visit(ConstantDeclarationNode &node) override;
    void visit(FieldNode &node) override;
    void visit(ParameterNode &node) override;
    void visit(VariableDeclarationNode &node) override;

    void visit(TypeReferenceNode &node) override;
    void visit(ValueReferenceNode &node) override;

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

    static bool envFieldResolver(ValueReferenceNode *var, const std::string &field_name, TypeNode *field_type) ;

public:
    explicit LambdaLifter() : module_(), env_(), level_() { };
    ~LambdaLifter() override = default;

    void run(Logger *logger, Node* node) override;

};


#endif //OBERON_LLVM_LAMBDALIFTER_H
