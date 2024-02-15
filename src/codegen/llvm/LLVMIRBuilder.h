/*
 * Simple tree-walk implementation that builds LLVM IR for the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 2/7/20.
 */

#ifndef OBERON0C_LLVMCODEGEN_H
#define OBERON0C_LLVMCODEGEN_H


#include <map>
#include <stack>
#include <string>
#include <vector>

#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "compiler/CompilerConfig.h"
#include "data/ast/ASTContext.h"
#include "data/ast/NodeVisitor.h"
#include "logging/Logger.h"
#include "system/PredefinedProcedure.h"

using namespace llvm;

using std::map;
using std::stack;
using std::string;
using std::vector;

class LLVMIRBuilder final : private NodeVisitor {

private:
    CompilerConfig &config_;
    Logger &logger_;
    IRBuilder<> builder_;
    Module *module_;
    Value *value_;
    map<DeclarationNode*, Value*> values_;
    map<TypeNode*, Type*> types_;
    map<ProcedureNode*, Function*> functions_;
    map<string, Constant*> strings_;
    stack<bool> deref_ctx;
    unsigned int level_;
    Function *function_;
    AttrBuilder attrs_;
    ASTContext *ast_;

    Type* getLLVMType(TypeNode *type);
    MaybeAlign getLLVMAlign(TypeNode *type);

    string qualifiedName(DeclarationNode *) const;

    void setRefMode(bool deref);
    void restoreRefMode();
    bool deref() const;

    Value *callPredefined(PredefinedProcedure *, QualIdent *, ActualParameters *, vector<Value *> &);

    void cast(ExpressionNode &);

    void proc(ProcedureNode &);

    using Selectors = vector<unique_ptr<Selector>>;
    using SelectorIterator = Selectors::iterator;
    TypeNode *selectors(TypeNode *, SelectorIterator, SelectorIterator);
    void parameters(ProcedureTypeNode *, ActualParameters *, vector<Value *> &);
    TypeNode *call(ProcedureNode *, QualIdent *, Selectors &);

    void visit(ModuleNode &) override;
    void visit(ProcedureNode &) override;

    void visit(ImportNode &) override;

    void visit(ConstantDeclarationNode &) override;
    void visit(FieldNode &) override;
    void visit(ParameterNode &) override;
    void visit(VariableDeclarationNode &) override;

    void visit(QualifiedStatement &) override;
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
