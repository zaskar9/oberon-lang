/*
 * Header of the simple tree-walk code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_CODEGENERATOR_H
#define OBERON0C_CODEGENERATOR_H


#include <unordered_map>
#include "NasmAssembly.h"
#include "../parser/ast/NodeVisitor.h"

class CodeGenerator final : NodeVisitor {

private:
    NasmAssembly* assembly_;

public:
    explicit CodeGenerator(NasmAssembly *assembly) : assembly_(assembly) { };
    ~CodeGenerator() = default;

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

};

#endif //OBERON0C_CODEGENERATOR_H
