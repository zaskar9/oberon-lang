/*
 * Interface of a node visitor for the abstract syntax tree used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/31/2018.
 */

#ifndef OBERON0C_NODEVISITOR_H
#define OBERON0C_NODEVISITOR_H


#include "ModuleNode.h"
#include "DeclarationNode.h"
#include "UnaryExpressionNode.h"
#include "BinaryExpressionNode.h"
#include "ReferenceNode.h"
#include "NumberNode.h"
#include "StringNode.h"
#include "BooleanNode.h"
#include "ArrayTypeNode.h"
#include "RecordTypeNode.h"
#include "ProcedureCallNode.h"
#include "AssignmentNode.h"
#include "IfThenElseNode.h"
#include "LoopNode.h"

class NodeVisitor {

public:
    virtual void visit(ModuleNode &node) = 0;
    virtual void visit(ProcedureNode &node) = 0;

    virtual void visit(ReferenceNode &node) = 0;
    virtual void visit(ConstantDeclarationNode &node) = 0;
    virtual void visit(FieldNode &node) = 0;
    virtual void visit(ParameterNode &node) = 0;
    virtual void visit(TypeDeclarationNode &node) = 0;
    virtual void visit(VariableDeclarationNode &node) = 0;

    virtual void visit(BooleanNode &node) = 0;
    virtual void visit(NumberNode &node) = 0;
    virtual void visit(StringNode &node) = 0;
    virtual void visit(UnaryExpressionNode &node) = 0;
    virtual void visit(BinaryExpressionNode &node) = 0;

    virtual void visit(ArrayTypeNode &node) = 0;
    virtual void visit(BasicTypeNode &node) = 0;
    virtual void visit(RecordTypeNode &node) = 0;

    virtual void visit(StatementSequenceNode &node) = 0;
    virtual void visit(AssignmentNode &node) = 0;
    virtual void visit(IfThenElseNode &node) = 0;
    virtual void visit(ElseIfNode &node) = 0;
    virtual void visit(ProcedureCallNode &node) = 0;
    virtual void visit(LoopNode &node) = 0;
    virtual void visit(WhileLoopNode &node) = 0;
    virtual void visit(RepeatLoopNode &node) = 0;
    virtual void visit(ForLoopNode &node) = 0;

};


#endif //OBERON0C_NODEVISITOR_H
