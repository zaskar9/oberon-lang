/*
 * Implementation of the simple tree-walk code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "CodeGenerator.h"

void CodeGenerator::visit(ModuleNode& node) {
    // create BSS (Block Started by Symbol) section
    if (node.getVariableCount() > 0) {
        auto bss = assembly_->getBssSection();
        // reserve space for global variables
        for (size_t i = 0; i < node.getVariableCount(); i++) {
            auto var = node.getVariable(i);
            auto instruction = std::make_unique<Instruction>(OpCode::res, OpMode::b8,nullptr); //, new Immediate(var->getType()->getSize()));
            std::string label = "_" + var->getName();
            instruction->setLabel(label);
            bss->addInstruction(std::move(instruction));
        }
    }
    // visit statement sequence
    node.getStatements()->accept(*this);
}

void CodeGenerator::visit(ProcedureNode& node) { }

void CodeGenerator::visit(ReferenceNode& node) { }

void CodeGenerator::visit(ConstantDeclarationNode& node) { }

void CodeGenerator::visit(FieldNode& node) { }

void CodeGenerator::visit(ParameterNode& node) { }

void CodeGenerator::visit(VariableDeclarationNode& node) { }

void CodeGenerator::visit(BooleanLiteralNode& node) { }

void CodeGenerator::visit(IntegerLiteralNode& node) { }

void CodeGenerator::visit(StringLiteralNode& node) { }

void CodeGenerator::visit(UnaryExpressionNode& node) { }

void CodeGenerator::visit(BinaryExpressionNode& node) { }

void CodeGenerator::visit(TypeDeclarationNode& node) { }

void CodeGenerator::visit(ArrayTypeNode& node) { }

void CodeGenerator::visit(BasicTypeNode& node) { }

void CodeGenerator::visit(RecordTypeNode& node) { }

void CodeGenerator::visit(StatementSequenceNode& node) { }

void CodeGenerator::visit(AssignmentNode& node) { }

void CodeGenerator::visit(IfThenElseNode& node) { }

void CodeGenerator::visit(ElseIfNode& node) { }

void CodeGenerator::visit(ProcedureCallNode& node) { }

void CodeGenerator::visit(LoopNode& node) { }

void CodeGenerator::visit(WhileLoopNode& node) { }

void CodeGenerator::visit(RepeatLoopNode& node) { }

void CodeGenerator::visit(ForLoopNode& node) { }