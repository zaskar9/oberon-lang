/*
 * Simple tree-walk code generator to produce LLVM assembly for the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#ifndef OBERON0C_LLVMCODEGEN_H
#define OBERON0C_LLVMCODEGEN_H


#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
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
    bool deref_;
    int level_;
    Function* function_;

    Type* getLLVMType(TypeNode* type, bool isPtr = false);
    void call(CallNode &node);

public:
    explicit LLVMCodeGen(Logger* logger) : logger_(logger), context_(), builder_(context_), module_(),
            value_(), values_(), deref_(false), level_(0), function_() { };
    ~LLVMCodeGen() = default;

    Module* getModule() const;

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

};


#endif //OBERON0C_LLVMCODEGEN_H
