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
#include <unordered_set>
#include <vector>

#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/TargetParser/Triple.h>

#include "Logger.h"
#include "compiler/CompilerConfig.h"
#include "data/ast/ASTContext.h"
#include "data/ast/NodeVisitor.h"
#include "system/PredefinedProcedure.h"

using namespace llvm;

using std::map;
using std::stack;
using std::string;
using std::unordered_set;
using std::vector;

class LLVMIRBuilder final : private NodeVisitor {

private:
    CompilerConfig &config_;
    Logger &logger_;
    IRBuilder<> builder_;
    Module *module_;
    Triple triple_;
    Value *value_;
    map<DeclarationNode*, Value*> values_;
    map<TypeNode*, Type*> types_;
    map<ArrayTypeNode*, Value*> typeDopes_;
    map<DeclarationNode*, Value*> valueDopes_;
    map<ProcedureTypeNode *, FunctionType *> funTypes_;
    map<PointerTypeNode*, StructType*> ptrTypes_;
    map<RecordTypeNode*, GlobalValue*> recTypeIds_;
    map<RecordTypeNode*, GlobalValue*> recTypeTds_;
    map<DeclarationNode *, Value *> valueTds_;
    stack<BasicBlock *> loopTails_;
    map<ProcedureNode*, Function*> functions_;
    map<string, Constant*> strings_;
    stack<bool> deref_ctx;
    unsigned int scope_;
    vector<string> scopes_;
    Function *function_;
    AttrBuilder attrs_;
    ASTContext *ast_;
    StructType *recordTdTy_;

    Type *getLLVMType(TypeNode *type);
    MaybeAlign getLLVMAlign(TypeNode *type);

    Value *processGEP(Type *, Value *, vector<Value *> &);

    Value *getArrayLength(ExpressionNode *, uint32_t);
    Value *getOpenArrayLength(Value *, ArrayTypeNode *, uint32_t, bool = true);
    Value *getDopeVector(ExpressionNode *);
    Value *getDopeVector(NodeReference *, TypeNode *);

    Value *getTypeDescriptor(Value *, NodeReference *, TypeNode *);

    string qualifiedName(DeclarationNode *) const;
    string createScopedName(TypeNode *) const;

    void setRefMode(bool deref);
    void restoreRefMode();
    bool deref() const;

    void ensureTerminator(BasicBlock *);

    void cast(ExpressionNode &);

    void procedure(ProcedureNode &);

    using Selectors = vector<unique_ptr<Selector>>;
    using SelectorIterator = Selectors::iterator;
    TypeNode *selectors(NodeReference *, TypeNode *, SelectorIterator, SelectorIterator);
    void parameters(ProcedureTypeNode *, ActualParameters *, vector<Value *> &, bool = false);

    void installTrap(Value *, uint8_t);
    void trapOutOfBounds(Value *, Value *, Value *);
    void trapTypeGuard(Value *);
    void trapCopyOverflow(Value *, Value *);
    void trapNILPtr(Value *);
    void trapIntDivByZero(Value *);
    void trapAssert(Value *);
    Value *trapIntOverflow(Intrinsic::IndependentIntrinsics, Value*, Value*);
    void trapFltDivByZero(Value *);

    Value *createTypeTest(Value *, TypeNode *);
    Value *createTypeTest(Value *, NodeReference *, TypeNode *, TypeNode *);

    void createNumericTestCase(CaseOfNode &, BasicBlock *, BasicBlock *);
    void createTypeTestCase(CaseOfNode &, BasicBlock *, BasicBlock *);

    Value *createStringComparison(BinaryExpressionNode *);

    Value *createNeg(Value *);
    Value *createAdd(Value *, Value *);
    Value *createSub(Value *, Value *);
    Value *createMul(Value *, Value *);
    Value *createDiv(Value *, Value *);
    Value *createMod(Value *, Value *);
    Value *createFDiv(Value *, Value *);

    TypeNode *createStaticCall(NodeReference &,  QualIdent *, Selectors &);
    Value *createPredefinedCall(PredefinedProcedure *, QualIdent *,
                                vector<unique_ptr<ExpressionNode>> &, vector<Value *> &);
    Value *createAbortCall();
    Value *createAbsCall(TypeNode *, Value *);
    Value *createAsrCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createAssertCall(Value *);
    Value *createChrCall(Value *);
    Value *createEntireCall(Value *);
    Value *createFltCall(Value *);
    Value *createPackCall(Value *, Value *);
    Value *createUnpkCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createExitCall(Value *);
    Value *createExclCall(Value *, Value *);
    Value *createDisposeCall(TypeNode *, Value *);
    Value *createIncDecCall(ProcKind, vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createInclCall(Value *, Value *);
    Value *createLenCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createLongCall(ExpressionNode *, Value *);
    Value *createLslCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createMaxMinCall(ExpressionNode *, bool);
    Value *createNewCall(TypeNode *, Value *);
    Value *createOddCall(Value *);
    Value *createOrdCall(ExpressionNode *, Value *);
    Value *createRorCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createShortCall(ExpressionNode *, Value *);
    Value *createSizeCall(ExpressionNode *);
    
    Value *createSystemAdrCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createSystemGetCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createSystemPutCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createSystemBitCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    Value *createSystemCopyCall(Value *, Value *, Value *);
    Value *createSystemValCall(vector<unique_ptr<ExpressionNode>> &, std::vector<Value *> &);
    
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
    void visit(CharLiteralNode &) override;
    void visit(NilLiteralNode &) override;
    void visit(SetLiteralNode &) override;
    void visit(RangeLiteralNode &) override;

    void visit(UnaryExpressionNode &) override;
    void visit(BinaryExpressionNode &) override;
    void visit(RangeExpressionNode &) override;
    void visit(SetExpressionNode &) override;

    void visit(TypeDeclarationNode &) override;
    void visit(ArrayTypeNode &) override;
    void visit(BasicTypeNode &) override;
    void visit(ProcedureTypeNode &) override;
    void visit(RecordTypeNode &) override;
    void visit(PointerTypeNode &) override;

    void visit(StatementSequenceNode &) override;
    void visit(AssignmentNode &) override;
    void visit(CaseOfNode &) override;
    void visit(CaseLabelNode &) override;
    void visit(CaseNode &) override;
    void visit(IfThenElseNode &) override;
    void visit(ElseIfNode &) override;
    void visit(LoopNode &) override;
    void visit(WhileLoopNode &) override;
    void visit(RepeatLoopNode &) override;
    void visit(ForLoopNode &) override;
    void visit(ReturnNode &) override;
    void visit(ExitNode &) override;

public:
    LLVMIRBuilder(CompilerConfig &config, LLVMContext &builder, Module *module);
    ~LLVMIRBuilder() override = default;

    void build(ASTContext *ast);

};


#endif //OBERON0C_LLVMCODEGEN_H
