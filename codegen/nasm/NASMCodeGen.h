/*
 * Simple tree-walk code generator to produce NASM assembly for the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_NASMCODEGEN_H
#define OBERON0C_NASMCODEGEN_H


#include <unordered_map>
#include "Assembly.h"
#include "../../parser/ast/NodeVisitor.h"

class NASMCodeGen final : NodeVisitor {

private:
    Assembly* assembly_;
    const Register* result_;
    std::vector<std::unique_ptr<Register>> registers_;
    bool free_[16];
    bool lValue_;

    const Register* allocRegister();
    void freeRegister(const Register* reg);

    OpMode getMode(const TypeNode* type) const;


public:
    explicit NASMCodeGen(Assembly *assembly);
    ~NASMCodeGen() = default;

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

#endif //OBERON0C_NASMCODEGEN_H
