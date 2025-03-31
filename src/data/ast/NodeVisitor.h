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
#include "PointerTypeNode.h"
#include "AssignmentNode.h"
#include "CaseOfNode.h"
#include "IfThenElseNode.h"
#include "LoopNode.h"

class NodeVisitor {

public:
    virtual void visit(ModuleNode &) = 0;
    virtual void visit(ProcedureNode &) = 0;

    virtual void visit(ImportNode &) = 0;

    virtual void visit(ConstantDeclarationNode &) = 0;
    virtual void visit(FieldNode &) = 0;
    virtual void visit(ParameterNode &) = 0;
    virtual void visit(TypeDeclarationNode &) = 0;
    virtual void visit(VariableDeclarationNode &) = 0;

    virtual void visit(QualifiedStatement &) = 0;
    virtual void visit(QualifiedExpression &) = 0;

    virtual void visit(BooleanLiteralNode &) = 0;
    virtual void visit(IntegerLiteralNode &) = 0;
    virtual void visit(RealLiteralNode &) = 0;
    virtual void visit(StringLiteralNode &) = 0;
    virtual void visit(CharLiteralNode &) = 0;
    virtual void visit(NilLiteralNode &) = 0;
    virtual void visit(SetLiteralNode &) = 0;
    virtual void visit(RangeLiteralNode &) = 0;

    virtual void visit(UnaryExpressionNode &) = 0;
    virtual void visit(BinaryExpressionNode &) = 0;
    virtual void visit(RangeExpressionNode &) = 0;
    virtual void visit(SetExpressionNode &) = 0;

    virtual void visit(ArrayTypeNode &) = 0;
    virtual void visit(BasicTypeNode &) = 0;
    virtual void visit(ProcedureTypeNode &) = 0;
    virtual void visit(RecordTypeNode &) = 0;
    virtual void visit(PointerTypeNode &) = 0;

    virtual void visit(StatementSequenceNode &) = 0;
    virtual void visit(AssignmentNode &) = 0;
    virtual void visit(CaseOfNode &) = 0;
    virtual void visit(CaseLabelNode &) = 0;
    virtual void visit(CaseNode &) = 0;
    virtual void visit(IfThenElseNode &) = 0;
    virtual void visit(ElseIfNode &) = 0;
    virtual void visit(LoopNode &) = 0;
    virtual void visit(WhileLoopNode &) = 0;
    virtual void visit(RepeatLoopNode &) = 0;
    virtual void visit(ForLoopNode &) = 0;
    virtual void visit(ReturnNode &) = 0;
    virtual void visit(ExitNode &) = 0;

    virtual ~NodeVisitor() noexcept;

};


#endif //OBERON0C_NODEVISITOR_H
