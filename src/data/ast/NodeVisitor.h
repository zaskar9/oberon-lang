/*
 * Node visitor for the abstract syntax tree used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/31/2018.
 */

#ifndef OBERON0C_NODEVISITOR_H
#define OBERON0C_NODEVISITOR_H


#include "ModuleNode.h"
#include "DeclarationNode.h"
#include "NodeReference.h"
#include "ArrayTypeNode.h"
#include "RecordTypeNode.h"
#include "AssignmentNode.h"
#include "IfThenElseNode.h"
#include "LoopNode.h"
#include "PointerTypeNode.h"

class NodeVisitor {

public:
    virtual void visit(ModuleNode &node) = 0;
    virtual void visit(ProcedureNode &node) = 0;

    virtual void visit(ImportNode &node) = 0;

    virtual void visit(ConstantDeclarationNode &node) = 0;
    virtual void visit(FieldNode &node) = 0;
    virtual void visit(ParameterNode &node) = 0;
    virtual void visit(TypeDeclarationNode &node) = 0;
    virtual void visit(VariableDeclarationNode &node) = 0;

    virtual void visit(ValueReferenceNode &node) = 0;
    virtual void visit(TypeReferenceNode &node) = 0;

    virtual void visit(BooleanLiteralNode &node) = 0;
    virtual void visit(IntegerLiteralNode &node) = 0;
    virtual void visit(StringLiteralNode &node) = 0;
    virtual void visit(NilLiteralNode &node) = 0;

    virtual void visit(UnaryExpressionNode &node) = 0;
    virtual void visit(BinaryExpressionNode &node) = 0;

    virtual void visit(ArrayTypeNode &node) = 0;
    virtual void visit(BasicTypeNode &node) = 0;
    virtual void visit(ProcedureTypeNode &node) = 0;
    virtual void visit(RecordTypeNode &node) = 0;
    virtual void visit(PointerTypeNode &node) = 0;

    virtual void visit(StatementSequenceNode &node) = 0;
    virtual void visit(AssignmentNode &node) = 0;
    virtual void visit(IfThenElseNode &node) = 0;
    virtual void visit(ElseIfNode &node) = 0;
    virtual void visit(ProcedureCallNode &node) = 0;
    virtual void visit(LoopNode &node) = 0;
    virtual void visit(WhileLoopNode &node) = 0;
    virtual void visit(RepeatLoopNode &node) = 0;
    virtual void visit(ForLoopNode &node) = 0;
    virtual void visit(ReturnNode &node) = 0;

    virtual ~NodeVisitor() noexcept;

};


#endif //OBERON0C_NODEVISITOR_H
