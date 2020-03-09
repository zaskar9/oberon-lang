/*
 * Analysis pass that removes nested procedures used the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/9/20.
 */

#include "LambdaLifter.h"

void LambdaLifter::run(Logger *logger, Node *node) {
    if (logger->getErrorCount() == 0) {
        node->accept(*this);
    }
}

void LambdaLifter::visit([[maybe_unused]] ModuleNode &node) {
    module_ = &node;
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
}

void LambdaLifter::visit(ProcedureNode &node) {
    std::cout << node.getName() << ": " << node.getLevel() << std::endl;
    if (node.getProcedureCount() > 0) {
        // create a new record type with all parameters and variables
        FilePos pos = { "", 0, 0 };
        std::string name = "_env." + node.getName();
        auto record_t = std::make_unique<RecordTypeNode>(pos, name);
        for (size_t i = 0; i < node.getParameterCount(); i++) {
            auto param = node.getParameter(i);
            record_t->addField(std::make_unique<FieldNode>(pos, param->getName(), param->getType()));
        }
        for (size_t i = 0; i < node.getVariableCount(); i++) {
            auto var = node.getVariable(i);
            record_t->addField(std::make_unique<FieldNode>(pos, var->getName(), var->getType()));
        }
        auto type = record_t.get();
        module_->registerType(std::move(record_t));
        module_->addTypeDeclaration(std::make_unique<TypeDeclarationNode>(pos, name, type));
    }

    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
}

void LambdaLifter::visit([[maybe_unused]] ConstantDeclarationNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] FieldNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] ParameterNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] VariableDeclarationNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] TypeReferenceNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] ValueReferenceNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] BooleanLiteralNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] IntegerLiteralNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] StringLiteralNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] FunctionCallNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] UnaryExpressionNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] BinaryExpressionNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] TypeDeclarationNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] ArrayTypeNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] BasicTypeNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] RecordTypeNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] StatementSequenceNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] AssignmentNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] IfThenElseNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] ElseIfNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] ProcedureCallNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] LoopNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] WhileLoopNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] RepeatLoopNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] ForLoopNode &node) {

}

void LambdaLifter::visit([[maybe_unused]] ReturnNode &node) {

}
