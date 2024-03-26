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
        if (!node.getType()->parameters().empty() || node.getVariableCount() > 0) {
            // create a record type for the procedure's environment, containing fields for all parameters and variables
            auto identifier = make_unique<IdentDef>("_T" + node.getIdentifier()->name());
            vector<unique_ptr<FieldNode>> fields;
            for (auto &param : node.getType()->parameters()) {
                auto ident = make_unique<IdentDef>(param->getIdentifier()->name());
                fields.push_back(make_unique<FieldNode>(EMPTY_POS, std::move(ident), param->getType()));
            }
            for (size_t i = 0; i < node.getVariableCount(); i++) {
                auto var = node.getVariable(i);
                auto ident = make_unique<IdentDef>(var->getIdentifier()->name());
                fields.push_back(make_unique<FieldNode>(EMPTY_POS, std::move(ident), var->getType()));
            }
            auto type = context_->getOrInsertRecordType(EMPTY_POS, EMPTY_POS, std::move(fields));
            auto decl = make_unique<TypeDeclarationNode>(EMPTY_POS, std::move(identifier), type);
            decl->setLevel(module_->getLevel() + 1);
            module_->addTypeDeclaration(std::move(decl));
            // insert an additional formal parameter to the sub-procedures of the procedure to pass its environment
            for (auto &proc : node.procedures()) {
                auto param = make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>(SUPER_), type, true);
                param->setLevel(proc->getLevel() + 1);
                proc->getType()->parameters().push_back(std::move(param));
            }
            // create local variable to manage for the procedure's environment (this)
            auto var = make_unique<VariableDeclarationNode>(EMPTY_POS, make_unique<IdentDef>(THIS_), type);
            var->setLevel(level_ + 1);
            env_ = var.get();
            // initialize the procedure's environment (this)
            for (size_t i = 0; i < node.getType()->parameters().size(); i++) {
                auto param = node.getType()->parameters()[i].get();
                auto lhs = make_unique<QualifiedExpression>(env_);
                auto field = type->getField(param->getIdentifier()->name());
                lhs->selectors().push_back(make_unique<RecordField>(EMPTY_POS, field));
                lhs->setType(field->getType());
                auto rhs = make_unique<QualifiedExpression>(param);
                node.statements()->insertStatement(i, make_unique<AssignmentNode>(EMPTY_POS, std::move(lhs), std::move(rhs)));
            }
            node.insertVariable(0, std::move(var));
            // alter the statements of the procedure to use the procedure's environment (this)
            for (size_t i = node.getType()->parameters().size(); i < node.statements()->getStatementCount(); i++) {
                node.statements()->getStatement(i)->accept(*this);
            }
            // append statements to write values of var-parameters back from procedure's environment (this)
            for (auto &param : node.getType()->parameters()) {
                if (param->isVar() && param->getIdentifier()->name() != SUPER_) {
                    auto lhs = make_unique<QualifiedExpression>(param.get());
                    auto rhs = make_unique<QualifiedExpression>(env_);
                    auto field = type->getField(param->getIdentifier()->name());
                    rhs->selectors().push_back(make_unique<RecordField>(EMPTY_POS, field));
                    rhs->setType(field->getType());
                    node.statements()->addStatement(make_unique<AssignmentNode>(EMPTY_POS, std::move(lhs), std::move(rhs)));
                }
            }
            // write updates back to super-procedure's environment (super := this.super)
            if (level_ > SymbolTable::MODULE_LEVEL) /* neither root, nor leaf procedure */ {
                auto param = findParameter(SUPER_, node.getType()->parameters());
                if (param) {
                    auto lhs = make_unique<QualifiedExpression>(param);
                    auto rhs = make_unique<QualifiedExpression>(env_);
                    auto field = type->getField(SUPER_);
                    rhs->selectors().push_back(make_unique<RecordField>(EMPTY_POS, field));
                    rhs->setType(field->getType());
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
        if ((env_ = findParameter(SUPER_, node.getType()->parameters()))) {
            for (size_t i = 0; i < node.statements()->getStatementCount(); i++) {
                node.statements()->getStatement(i)->accept(*this);
            }
        }
    }
    if (level_ > SymbolTable::MODULE_LEVEL) {
        level_ = module_->getLevel() + 1;
        node.setLevel(level_);
        level_++;
        for (auto &param : node.getType()->parameters()) {
            param->accept(*this);
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

ParameterNode *LambdaLifter::findParameter(string name, vector<unique_ptr<ParameterNode>> &params) {
    for (auto &param : params) {
        if (param->getIdentifier()->name() == name) {
            return param.get();
        }
    }
    return nullptr;
}

void LambdaLifter::visit(ImportNode &) {}

void LambdaLifter::visit(ConstantDeclarationNode &node) {
    node.setLevel(level_);
}

void LambdaLifter::visit(FieldNode &) {}

void LambdaLifter::visit(ParameterNode &node) {
    node.setLevel(level_);
}

void LambdaLifter::visit(VariableDeclarationNode &node) {
    node.setLevel(level_);
}

// TODO copy-paste from visit(ProcedureNodeReference &)
void LambdaLifter::visit(QualifiedStatement &node) {
    auto decl = node.dereference();
    selectors(decl->getType(), node.selectors());
}

void LambdaLifter::visit(QualifiedExpression &node) {
    auto decl = node.dereference();
    if (decl->getLevel() == SymbolTable::MODULE_LEVEL ||
        (env_->getIdentifier()->name() == SUPER_ && env_->getLevel() == decl->getLevel())) {
        // global variable or local variable in leaf procedure
        return;
    }
    selectors(decl->getType(), node.selectors());
    if (decl->getNodeType() == NodeType::parameter || decl->getNodeType() == NodeType::variable) {
        node.resolve(env_);
        if (!envFieldResolver(&node, decl->getIdentifier()->name(), decl->getType())) {
            std::cerr << "Unable to resolve record field: " << *decl->getIdentifier() << "." << std::endl;
        }
    }
}

void LambdaLifter::selectors(TypeNode *base, vector<unique_ptr<Selector>> &selectors) {
    for (auto &sel : selectors) {
        auto selector = sel.get();
        if (selector->getNodeType() == NodeType::parameter) {
            // process regular parameters
            auto params = dynamic_cast<ActualParameters *>(selector);
            for (auto &param : params->parameters()) {
                param->accept(*this);
            }
            auto type = dynamic_cast<ProcedureTypeNode *>(base);
            // process procedure environment parameter
            if (type->parameters().size() > params->parameters().size()) {
                auto param = make_unique<QualifiedExpression>(env_);
                params->parameters().push_back(std::move(param));
            }
            base = type->getReturnType();
        } else if (selector->getNodeType() == NodeType::array_type) {
            auto indices = dynamic_cast<ArrayIndex *>(selector);
            auto type = dynamic_cast<ArrayTypeNode *>(base);
            for (auto &index : indices->indices()) {
                index->accept(*this);
            }
            base = type->getMemberType();
        } else if (selector->getNodeType() == NodeType::pointer_type) {
            auto type = dynamic_cast<PointerTypeNode *>(base);
            base = type->getBase();
        } else if (selector->getNodeType() == NodeType::record_type) {
            auto field = dynamic_cast<RecordField *>(selector);
            base = field->getField()->getType();
        } else if (selector->getNodeType() == NodeType::type) {
            auto guard = dynamic_cast<Typeguard *>(selector);
            base = guard->getType();
        }
    }
}

void LambdaLifter::visit(BooleanLiteralNode &) {}

void LambdaLifter::visit(IntegerLiteralNode &) {}

void LambdaLifter::visit(RealLiteralNode &) {}

void LambdaLifter::visit(StringLiteralNode &) {}

void LambdaLifter::visit(CharLiteralNode &) {}

void LambdaLifter::visit(NilLiteralNode &) {}

void LambdaLifter::visit(SetLiteralNode &) {}

void LambdaLifter::visit(RangeLiteralNode &) {}

void LambdaLifter::visit(UnaryExpressionNode &node) {
    node.getExpression()->accept(*this);
}

void LambdaLifter::visit(BinaryExpressionNode &node) {
    node.getLeftExpression()->accept(*this);
    node.getRightExpression()->accept(*this);
}

void LambdaLifter::visit(RangeExpressionNode &node) {
    node.getLower()->accept(*this);
    node.getUpper()->accept(*this);
}

void LambdaLifter::visit(SetExpressionNode &node) {
    for (auto &element : node.elements()) {
        element->accept(*this);
    }
}

void LambdaLifter::visit(TypeDeclarationNode &node) {
    node.setLevel(level_);
}

void LambdaLifter::visit(ArrayTypeNode &) {}

void LambdaLifter::visit(BasicTypeNode &) {}

void LambdaLifter::visit(ProcedureTypeNode &) {}

void LambdaLifter::visit(RecordTypeNode &) {}

void LambdaLifter::visit(PointerTypeNode &) {}

void LambdaLifter::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        node.getStatement(i)->accept(*this);
    }
}

void LambdaLifter::visit(AssignmentNode &node) {
    node.getLvalue()->accept(*this);
    node.getRvalue()->accept(*this);
}

void LambdaLifter::visit(IfThenElseNode &node) {
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

bool LambdaLifter::envFieldResolver(QualifiedExpression *var, const std::string &field_name, TypeNode *field_type) {
    auto type = dynamic_cast<RecordTypeNode*>(var->getType());
    auto &selectors = var->selectors();
    long num = 0;
    while (true) {
        auto field = type->getField(field_name);
        if (field && field->getType() == field_type) {
            selectors.insert(selectors.begin() + num, make_unique<RecordField>(EMPTY_POS, field));
            var->setType(field->getType());
            return true;
        } else {
            field = type->getField(SUPER_);
            if (field) {
                selectors.insert(selectors.begin() + num, make_unique<RecordField>(EMPTY_POS, field));
                type = dynamic_cast<RecordTypeNode *>(field->getType());
                num++;
            } else {
                return false;
            }
        }
    }
}
