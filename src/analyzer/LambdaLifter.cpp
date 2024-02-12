/*
 * Analysis pass that removes nested procedures used the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/9/20.
 */


#include "LambdaLifter.h"

#include <memory>
#include <string>
#include <vector>

#include "data/symtab/SymbolTable.h"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

const string LambdaLifter::THIS_ = "_this";
const string LambdaLifter::SUPER_ = "_super";

void LambdaLifter::run(Logger &logger, Node *node) {
    if (logger.getErrorCount() == 0) {
        node->accept(*this);
    }
}

void LambdaLifter::call(ProcedureNodeReference &node) {
    for (size_t i = 0; i < node.getActualParameterCount(); i++) {
        node.getActualParameter(i)->accept(*this);
    }
    auto proc = dynamic_cast<ProcedureNode *>(node.dereference());
    if (proc->getFormalParameterCount() > node.getActualParameterCount()) {
        auto param = make_unique<ValueReferenceNode>(EMPTY_POS, env_);
        if (proc->getLevel() != env_->getLevel()) {
            envFieldResolver(param.get(), SUPER_, proc->getFormalParameter(node.getActualParameterCount())->getType());
        }
        node.addActualParameter(std::move(param));
    }
}

void LambdaLifter::visit(ModuleNode &node) {
    module_ = &node;
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
}

void LambdaLifter::visit(ProcedureNode &node) {
    env_ = nullptr;
    level_ = node.getLevel();
    bool is_super = node.getProcedureCount();
    if (is_super) /* super procedure */ {
        if (node.getFormalParameterCount() > 0 || node.getVariableCount() > 0) {
            auto identifier = make_unique<IdentDef>("_T" + node.getIdentifier()->name());
            vector<unique_ptr<FieldNode>> fields;
            for (size_t i = 0; i < node.getFormalParameterCount(); i++) {
                auto param = node.getFormalParameter(i);
                auto ident = make_unique<IdentDef>(param->getIdentifier()->name());
                fields.push_back(make_unique<FieldNode>(EMPTY_POS, std::move(ident), param->getType()));
            }
            for (size_t i = 0; i < node.getVariableCount(); i++) {
                auto var = node.getVariable(i);
                auto ident = make_unique<IdentDef>(var->getIdentifier()->name());
                fields.push_back(make_unique<FieldNode>(EMPTY_POS, std::move(ident), var->getType()));
            }
            auto type = context_->getOrInsertRecordType(identifier.get(), std::move(fields));
            auto decl = make_unique<TypeDeclarationNode>(EMPTY_POS, std::move(identifier), type);
            decl->setLevel(module_->getLevel() + 1);
            module_->addTypeDeclaration(std::move(decl));
            for (size_t i = 0; i < node.getProcedureCount(); i++) {
                auto proc = node.getProcedure(i);
                auto param = make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>(SUPER_), type, true);
                param->setLevel(proc->getLevel() + 1);
                proc->addFormalParameter(std::move(param));
            }
            // create local variable to manage for the procedure's environment (this)
            auto var = make_unique<VariableDeclarationNode>(EMPTY_POS, make_unique<IdentDef>(THIS_), type);
            var->setLevel(level_ + 1);
            env_ = var.get();
            // initialize the procedure's environment (this)
            for (size_t i = 0; i < node.getFormalParameterCount(); i++) {
                auto param = node.getFormalParameter(i);
                auto lhs = make_unique<ValueReferenceNode>(EMPTY_POS, env_);
                auto field = type->getField(param->getIdentifier()->name());
                lhs->addSelector(make_unique<RecordField>(EMPTY_POS, field));
                auto rhs = make_unique<ValueReferenceNode>(EMPTY_POS, param);
                node.statements()->insertStatement(i, make_unique<AssignmentNode>(EMPTY_POS, std::move(lhs), std::move(rhs)));
            }
            node.insertVariable(0, std::move(var));
            // alter the statements of the procedure to use the procedure's environment (this)
            for (size_t i = node.getFormalParameterCount(); i < node.statements()->getStatementCount(); i++) {
                node.statements()->getStatement(i)->accept(*this);
            }
            // append statements to write values of var-parameters back from procedure's environment (this)
            for (size_t i = 0; i < node.getFormalParameterCount(); i++ ){
                auto param = node.getFormalParameter(i);
                if (param->isVar() && param != node.getFormalParameter(SUPER_)) {
                    auto lhs = make_unique<ValueReferenceNode>(EMPTY_POS, param);
                    auto rhs = make_unique<ValueReferenceNode>(EMPTY_POS, env_);
                    auto field = type->getField(param->getIdentifier()->name());
                    rhs->addSelector(make_unique<RecordField>(EMPTY_POS, field));
                    node.statements()->addStatement(make_unique<AssignmentNode>(EMPTY_POS, std::move(lhs), std::move(rhs)));
                }
            }
            // write updates back to super-procedure's environment (super := this.super)
            if (level_ > SymbolTable::MODULE_LEVEL) /* neither root, nor leaf procedure */ {
                auto param = node.getFormalParameter(SUPER_);
                if (param) {
                    auto lhs = make_unique<ValueReferenceNode>(EMPTY_POS, param);
                    auto rhs = make_unique<ValueReferenceNode>(EMPTY_POS, env_);
                    auto field = type->getField(SUPER_);
                    rhs->addSelector(make_unique<RecordField>(EMPTY_POS, field));
                    node.statements()->addStatement(make_unique<AssignmentNode>(EMPTY_POS, std::move(lhs), std::move(rhs)));
                }
            }
        }
        // rename and move the procedure to the module scope
        for (size_t i = 0; i < node.getProcedureCount(); i++) {
            auto proc = node.getProcedure(i);
            proc->setIdentifier(make_unique<IdentDef>("_" + proc->getIdentifier()->name()));
            module_->addProcedure(node.removeProcedure(i));
        }
        // TODO remove unnecessary local variables
        // node.removeVariables(1, node.getVariableCount());
    } else if (level_ > SymbolTable::MODULE_LEVEL) /* leaf procedure */ {
        if ((env_ = node.getFormalParameter(SUPER_))) {
            for (size_t i = 0; i < node.statements()->getStatementCount(); i++) {
                node.statements()->getStatement(i)->accept(*this);
            }
        }
    }
    if (level_ > SymbolTable::MODULE_LEVEL) {
        level_ = module_->getLevel() + 1;
        node.setLevel(level_);
        level_++;
        for (size_t i = 0; i < node.getFormalParameterCount(); i++) {
            node.getFormalParameter(i)->accept(*this);
        }
        for (size_t i = 0; i < node.getConstantCount(); i++) {
            node.getConstant(i)->accept(*this);
        }
        for (size_t i = 0; i < node.getTypeDeclarationCount(); i++) {
            node.getTypeDeclaration(i)->accept(*this);
        }
        for (size_t i = 0; i < node.getVariableCount(); i++) {
            node.getVariable(i)->accept(*this);
        }
    }
}

void LambdaLifter::visit([[maybe_unused]] ImportNode &node) { }

void LambdaLifter::visit(ConstantDeclarationNode &node) {
    node.setLevel(level_);
}

void LambdaLifter::visit([[maybe_unused]] FieldNode &node) { }

void LambdaLifter::visit(ParameterNode &node) {
    node.setLevel(level_);
}

void LambdaLifter::visit(VariableDeclarationNode &node) {
    node.setLevel(level_);
}

void LambdaLifter::visit(ValueReferenceNode &node) {
    if (node.getNodeType() == NodeType::procedure_call) {
        call(node);
        return;
    }
    auto decl = node.dereference();
    if (decl->getLevel() == SymbolTable::MODULE_LEVEL ||
        (env_->getIdentifier()->name() == SUPER_ && env_->getLevel() == decl->getLevel())) {
        // global variable or local variable in leaf procedure
        return;
    }
    for (size_t i = 0; i < node.getSelectorCount(); i++) {
        auto selector = node.getSelector(i);
        if (selector->getType() == NodeType::array_type) {
            auto indices = dynamic_cast<ArrayIndex *>(selector);
            for (auto& index : indices->indices()) {
                index->accept(*this);
            }
        }
    }
    node.resolve(env_);
    if (!envFieldResolver(&node, decl->getIdentifier()->name(), decl->getType())) {
        std::cerr << "Unable to resolve record field: " << decl->getIdentifier() << "." << std::endl;
    }
}


void LambdaLifter::visit([[maybe_unused]] BooleanLiteralNode &node) { }

void LambdaLifter::visit([[maybe_unused]] IntegerLiteralNode &node) { }

void LambdaLifter::visit([[maybe_unused]] RealLiteralNode &node) { }

void LambdaLifter::visit([[maybe_unused]] StringLiteralNode &node) { }

void LambdaLifter::visit([[maybe_unused]] NilLiteralNode &node) { }

void LambdaLifter::visit(UnaryExpressionNode &node) {
    node.getExpression()->accept(*this);
}

void LambdaLifter::visit(BinaryExpressionNode &node) {
    node.getLeftExpression()->accept(*this);
    node.getRightExpression()->accept(*this);
}

void LambdaLifter::visit(TypeDeclarationNode &node) {
    node.setLevel(level_);
}

void LambdaLifter::visit([[maybe_unused]] ArrayTypeNode &node) { }

void LambdaLifter::visit([[maybe_unused]] BasicTypeNode &node) { }

void LambdaLifter::visit([[maybe_unused]] ProcedureTypeNode &node) { }

void LambdaLifter::visit([[maybe_unused]] RecordTypeNode &node) { }

void LambdaLifter::visit([[maybe_unused]] PointerTypeNode &node) { }

void LambdaLifter::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        node.getStatement(i)->accept(*this);
    }
}

void LambdaLifter::visit(AssignmentNode &node) {
    node.getLvalue()->accept(*this);
    node.getRvalue()->accept(*this);
}

void LambdaLifter::visit([[maybe_unused]] IfThenElseNode &node) {
    node.getCondition()->accept(*this);
    node.getThenStatements()->accept(*this);
    if (node.hasElseIf()) {
        for (size_t i = 0; i < node.getElseIfCount(); i++) {
            node.getElseIf(i)->accept(*this);
        }
    }
    if (node.hasElse()) {
        node.getElseStatements()->accept(*this);
    }
}

void LambdaLifter::visit(ElseIfNode &node) {
    node.getCondition()->accept(*this);
    node.getStatements()->accept(*this);
}

void LambdaLifter::visit(ProcedureCallNode &node) {
    call(node);
}

void LambdaLifter::visit(LoopNode &node) {
    node.getStatements()->accept(*this);
}

void LambdaLifter::visit(WhileLoopNode &node) {
    node.getCondition()->accept(*this);
    node.getStatements()->accept(*this);
}

void LambdaLifter::visit(RepeatLoopNode &node) {
    node.getStatements()->accept(*this);
    node.getCondition()->accept(*this);
}

void LambdaLifter::visit(ForLoopNode &node) {
    node.getCounter()->accept(*this);
    node.getLow()->accept(*this);
    node.getHigh()->accept(*this);
    node.getStatements()->accept(*this);
}

void LambdaLifter::visit(ReturnNode &node) {
    node.getValue()->accept(*this);
}

bool LambdaLifter::envFieldResolver(ValueReferenceNode *var, const string &field_name, TypeNode *field_type) {
    auto type = dynamic_cast<RecordTypeNode*>(var->getType());
    size_t num = 0;
    while (true) {
        auto field = type->getField(field_name);
        if (field && field->getType() == field_type) {
            var->insertSelector(num, make_unique<RecordField>(EMPTY_POS, field));
            var->setType(field->getType());
            return true;
        } else {
            field = type->getField(SUPER_);
            if (field) {
                var->insertSelector(num, make_unique<RecordField>(EMPTY_POS, field));
                type = dynamic_cast<RecordTypeNode *>(field->getType());
                num++;
            } else {
                return false;
            }
        }
    }
}
