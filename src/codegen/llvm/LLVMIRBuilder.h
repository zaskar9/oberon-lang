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

#include "data/ast/ASTContext.h"
#include "compiler/CompilerConfig.h"
#include "data/ast/NodeVisitor.h"
#include "logging/Logger.h"

using namespace llvm;

class LLVMIRBuilder final : private NodeVisitor {

private:
    CompilerConfig &config_;
    Logger &logger_;
    IRBuilder<> builder_;
    Module *module_;
    Value *value_;
    std::map<DeclarationNode*, Value*> values_;
    std::map<TypeNode*, Type*> types_;
    std::map<ProcedureNode*, Function*> functions_;
    std::map<std::string, Constant*> strings_;
    std::stack<bool> deref_ctx;
    unsigned int level_;
    Function *function_;
    AttrBuilder attrs_;
    ASTContext *ast_;

    Type* getLLVMType(TypeNode *type);
    MaybeAlign getLLVMAlign(TypeNode *type);

    std::string qualifiedName(DeclarationNode *) const;

    void setRefMode(bool deref);
    void restoreRefMode();
    bool deref() const;

    Value *callPredefined(ProcedureNodeReference &, std::vector<Value *> &params);

    void cast(ExpressionNode &);

    void call(ProcedureNodeReference &);
    void proc(ProcedureNode &);

    void visit(ModuleNode &) override;
    void visit(ProcedureNode &) override;

    void visit(ImportNode &) override;

    void visit(ConstantDeclarationNode &) override;
    void visit(FieldNode &) override;
    void visit(ParameterNode &) override;
    void visit(VariableDeclarationNode &) override;

    void visit(ValueReferenceNode &) override;
    void visit(QualifiedExpression &) override;

    void visit(BooleanLiteralNode &) override;
    void visit(IntegerLiteralNode &) override;
    void visit(RealLiteralNode &) override;
    void visit(StringLiteralNode &) override;
    void visit(NilLiteralNode &) override;

    void visit(UnaryExpressionNode &) override;
    void visit(BinaryExpressionNode &) override;

    void visit(TypeDeclarationNode &) override;
    void visit(ArrayTypeNode &) override;
    void visit(BasicTypeNode &) override;
    void visit(ProcedureTypeNode &) override;
    void visit(RecordTypeNode &) override;
    void visit(PointerTypeNode &) override;

    void visit(StatementSequenceNode &) override;
    void visit(AssignmentNode &) override;
    void visit(IfThenElseNode &) override;
    void visit(ElseIfNode &) override;
    void visit(ProcedureCallNode &) override;
    void visit(LoopNode &) override;
    void visit(WhileLoopNode &) override;
    void visit(RepeatLoopNode &) override;
    void visit(ForLoopNode &) override;
    void visit(ReturnNode &) override;

public:
    LLVMIRBuilder(CompilerConfig &config, LLVMContext &builder, Module *module);
    ~LLVMIRBuilder() override = default;

    void build(ASTContext *ast);

};


#endif //OBERON0C_LLVMCODEGEN_H
