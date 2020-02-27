/*
 * Simple tree-walk code generator to produce LLVM assembly for the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#ifndef OBERON0C_LLVMCODEGEN_H
#define OBERON0C_LLVMCODEGEN_H


#include <stack>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include "../../parser/ast/NodeVisitor.h"

using namespace llvm;

class LLVMCodeGen final : NodeVisitor {

private:
    Logger *logger_;
    LLVMContext context_;
    IRBuilder<> builder_;
    std::unique_ptr<Module> module_;
    Value* value_;
    std::map<DeclarationNode*, Value*> values_;
    std::map<TypeNode*, Type*> types_;
    std::stack<bool> deref_ctx;
    int level_;
    Function *function_;
    TargetMachine *target_;

    Type* getLLVMType(TypeNode *type, bool isPtr = false);
    unsigned int getLLVMAlign(TypeNode *type, bool isPtr = false);

    void call(CallNode &node);

    void setRefMode(bool deref);
    void restoreRefMode();
    bool deref() const;

public:
    explicit LLVMCodeGen(Logger* logger);
    ~LLVMCodeGen() = default;

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

    Module* getModule() const;

};


#endif //OBERON0C_LLVMCODEGEN_H
