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

class LLVMIRBuilder final : NodeVisitor {

public:
    LLVMIRBuilder(CompilerConfig &config, LLVMContext &builder, Module *module);
    ~LLVMIRBuilder() override = default;

    void build(ASTContext *ast);

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
    map<string, Constant*> strings_;
    stack<bool> deref_ctx;
    unsigned int scope_;
    vector<string> scopes_;
    Function *function_;
    AttrBuilder attrs_;
    ASTContext *ast_{};
    StructType *recordTdTy_;

    Type *getLLVMType(TypeNode *type);
    MaybeAlign getLLVMAlign(TypeNode *type);

    Value *processGEP(Type *, Value *, vector<Value *> &);

    Value *getArrayLength(ExpressionNode *, uint32_t);
    Value *getOpenArrayLength(Value *, const ArrayTypeNode *, uint32_t, bool = true);
    Value *getDopeVector(ExpressionNode *);
    Value *getDopeVector(const NodeReference *, TypeNode *);

    Value *getTypeDescriptor(Value *, const NodeReference *, TypeNode *);

    string qualifiedName(DeclarationNode *) const;
    string createScopedName(const TypeNode *) const;

    void setRefMode(bool deref);
    void restoreRefMode();
    bool deref() const;

    void ensureTerminator(BasicBlock *);

    void cast(const ExpressionNode &);

    FunctionType *createFunctionType(ProcedureTypeNode &, CallingConvention);
    void createFunction(ProcedureNode &, CallingConvention);

    using Selectors = vector<unique_ptr<Selector>>;
    using SelectorIterator = Selectors::iterator;
    TypeNode *selectors(NodeReference *, TypeNode *, SelectorIterator, SelectorIterator);
    void parameters(ProcedureTypeNode *, ActualParameters *, vector<Value *> &, CallingConvention);

    void installTrap(Value *, uint8_t);
    void trapOutOfBounds(Value *, Value *, Value *);
    void trapTypeGuard(Value *);
    void trapCopyOverflow(Value *, Value *);
    void trapNILPtr(Value *);
    void trapIntDivByZero(Value *);
    void trapAssert(Value *);
    Value *trapIntOverflow(Intrinsic::IndependentIntrinsics, Value*, Value*);
    void trapFltDivByZero(Value *);
    void trapSignConversion(Value *);

    void checkSignConversion(ExpressionNode &, Value *);

    Value *createTypeTest(Value *, TypeNode *);
    Value *createTypeTest(Value *, const NodeReference *, const TypeNode *, TypeNode *);

    void createNumericTestCase(const CaseOfNode &, BasicBlock *, BasicBlock *);
    void createTypeTestCase(const CaseOfNode &, BasicBlock *, BasicBlock *);

    Value *createStringComparison(const BinaryExpressionNode *);

    Value *createNeg(Value *);
    Value *createAdd(Value *, Value *);
    Value *createSub(Value *, Value *);
    Value *createMul(Value *, Value *);
    Value *createDiv(Value *, Value *);
    Value *createMod(Value *, Value *);
    Value *createFDiv(Value *, Value *);

    TypeNode *createStaticCall(NodeReference &, const QualIdent *, Selectors &);
    Value *createPredefinedCall(const PredefinedProcedure *, const QualIdent *,
                                const vector<unique_ptr<ExpressionNode>> &, const vector<Value *> &);
    Value *createAbortCall();
    Value *createAbsCall(const TypeNode *, Value *);
    Value *createAsrCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createAssertCall(Value *);
    Value *createChrCall(Value *);
    Value *createEntireCall(Value *);
    Value *createFltCall(Value *);
    Value *createPackCall(Value *, Value *);
    Value *createUnpkCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createExitCall(Value *);
    Value *createExclCall(Value *, Value *);
    Value *createDisposeCall(TypeNode *, Value *);
    Value *createIncDecCall(ProcKind, const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createInclCall(Value *, Value *);
    Value *createLenCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createLongCall(const ExpressionNode *, Value *);
    Value *createLslCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createMaxMinCall(ExpressionNode *, bool);
    Value *createNewCall(TypeNode *, Value *);
    Value *createOddCall(Value *);
    Value *createOrdCall(const ExpressionNode *, Value *);
    Value *createRorCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createShortCall(const ExpressionNode *, Value *);
    Value *createSizeCall(ExpressionNode *);
    
    Value *createSystemAdrCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createSystemGetCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createSystemPutCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createSystemBitCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    Value *createSystemCopyCall(Value *, Value *, Value *);
    Value *createSystemValCall(const vector<unique_ptr<ExpressionNode>> &, const std::vector<Value *> &);
    
    void visit(ModuleNode &) override;
    void visit(ProcedureDeclarationNode &) override;
    void visit(ProcedureDefinitionNode &) override;

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

};


#endif //OBERON0C_LLVMCODEGEN_H
