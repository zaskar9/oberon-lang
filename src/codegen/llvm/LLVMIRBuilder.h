/*
 * Simple tree-walk implementation that builds LLVM IR for the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#ifndef OBERON0C_LLVMCODEGEN_H
#define OBERON0C_LLVMCODEGEN_H


#include <map>
#include <stack>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "data/ast/NodeVisitor.h"
#include "logging/Logger.h"

using namespace llvm;

class LLVMIRBuilder final : private NodeVisitor {

private:
    Logger *logger_;
    IRBuilder<> builder_;
    Module *module_;
    Value *value_;
    std::map<DeclarationNode*, Value*> values_;
    std::map<TypeNode*, Type*> types_;
    std::map<ProcedureNode*, Function*> functions_;
    std::stack<bool> deref_ctx;
    unsigned int level_;
    Function *function_;

    Type* getLLVMType(TypeNode *type, bool isPtr = false);
    MaybeAlign getLLVMAlign(TypeNode *type, bool isPtr = false);

    std::string qualifiedName(Ident *ident, bool external) const;

    void setRefMode(bool deref);
    void restoreRefMode();
    bool deref() const;

    Value *callBuiltIn(std::string name, std::vector<Value *> &params);

    void cast(ExpressionNode &node);

    void call(ProcedureNodeReference &node);
    void proc(ProcedureNode &node);

    void visit(ModuleNode &node) override;
    void visit(ProcedureNode &node) override;

    void visit(ImportNode &node) override;

    void visit(ConstantDeclarationNode &node) override;
    void visit(FieldNode &node) override;
    void visit(ParameterNode &node) override;
    void visit(VariableDeclarationNode &node) override;

    void visit(ValueReferenceNode &node) override;
    void visit(TypeReferenceNode &node) override;

    void visit(BooleanLiteralNode &node) override;
    void visit(IntegerLiteralNode &node) override;
    void visit(RealLiteralNode &node) override;
    void visit(StringLiteralNode &node) override;
    void visit(NilLiteralNode &node) override;

    void visit(UnaryExpressionNode &node) override;
    void visit(BinaryExpressionNode &node) override;

    void visit(TypeDeclarationNode &node) override;
    void visit(ArrayTypeNode &node) override;
    void visit(BasicTypeNode &node) override;
    void visit(ProcedureTypeNode &node) override;
    void visit(RecordTypeNode &node) override;
    void visit(PointerTypeNode &node) override;

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

public:
    explicit LLVMIRBuilder(Logger *logger, LLVMContext &context, Module *module);
    ~LLVMIRBuilder() = default;

    void build(Node *node);

};


#endif //OBERON0C_LLVMCODEGEN_H
