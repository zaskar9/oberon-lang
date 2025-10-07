//
// Created by Michael Grossniklaus on 12/17/23.
//

#ifndef OBERON_LANG_SEMA_H
#define OBERON_LANG_SEMA_H


#include <bitset>
#include <functional>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "Logger.h"
#include "data/ast/ASTContext.h"
#include "data/ast/CaseOfNode.h"
#include "data/ast/IfThenElseNode.h"
#include "data/ast/LoopNode.h"
#include "data/ast/ModuleNode.h"
#include "data/symtab/SymbolExporter.h"
#include "data/symtab/SymbolImporter.h"
#include "data/symtab/SymbolTable.h"
#include "system/OberonSystem.h"

using std::function;
using std::bitset;
using std::unordered_map;
using std::optional;
using std::stack;
using std::string;
using std::unique_ptr;
using std::vector;

class Sema {

public:
    Sema(CompilerConfig &, ASTContext *, OberonSystem *);
    Sema(const Sema&) = delete;
    Sema& operator=(const Sema&) = delete;

    void onBlockStart() const;
    void onBlockEnd() const;

    void onTranslationUnitStart(const FilePos &, const FilePos &, const unique_ptr<Ident> &) const;
    void onTranslationUnitEnd(const string &);

    unique_ptr<ModuleNode> onModuleStart(const FilePos &, unique_ptr<Ident>);
    void onModuleEnd(const FilePos&, const unique_ptr<Ident>&);

    unique_ptr<ImportNode> onImport(const FilePos &, const FilePos &, unique_ptr<Ident>, unique_ptr<Ident>);

    unique_ptr<ConstantDeclarationNode> onConstant(const FilePos &, const FilePos &,
                                                   unique_ptr<IdentDef>, unique_ptr<ExpressionNode>);

    unique_ptr<TypeDeclarationNode> onType(const FilePos &, const FilePos &,
                                           unique_ptr<IdentDef>, TypeNode *);
    ArrayTypeNode *onArrayType(const FilePos &, const FilePos &, const vector<unique_ptr<ExpressionNode>> &, TypeNode *) const;
    [[nodiscard]] PointerTypeNode *onPointerTypeStart(const FilePos &, const FilePos &) const;
    void onPointerTypeEnd(const FilePos &, const FilePos &, PointerTypeNode *, unique_ptr<QualIdent>);
    void onPointerTypeEnd(const FilePos &, const FilePos &, PointerTypeNode *, TypeNode *) const;
    ProcedureTypeNode *onProcedureType(const FilePos &, const FilePos &,
                                       vector<unique_ptr<ParameterNode>>, bool varargs, TypeNode *) const;
    unique_ptr<ParameterNode> onParameter(const FilePos &, const FilePos &,
                                          unique_ptr<Ident>, TypeNode *, bool, unsigned = 0) const;
    [[nodiscard]] RecordTypeNode *onRecordType(const FilePos &, const FilePos &, const unique_ptr<QualIdent> &,
                                               vector<unique_ptr<FieldNode>>) const;
    unique_ptr<FieldNode> onField(const FilePos&, const FilePos&, unique_ptr<IdentDef>, TypeNode*, unsigned = 0) const;

    [[nodiscard]] TypeNode *onTypeReference(const FilePos &, const FilePos &, const unique_ptr<QualIdent> &, unsigned = 0) const;

    unique_ptr<VariableDeclarationNode> onVariable(const FilePos &, const FilePos &,
                                                   unique_ptr<IdentDef>, TypeNode*, int = 0) const;

    void onDeclarations();

    unique_ptr<ProcedureDeclarationNode> onProcedureDeclaration(const FilePos &, const FilePos &,
                                                                unique_ptr<IdentDef>, ProcedureTypeNode *,
                                                                const string &, string &name) const;
    ProcedureDefinitionNode *onProcedureDefinitionStart(const FilePos &, unique_ptr<IdentDef>);
    unique_ptr<ProcedureDefinitionNode> onProcedureDefinitionEnd(const FilePos &, const unique_ptr<Ident> &);

    void onStatementSequence(const StatementSequenceNode *) const;

    unique_ptr<AssignmentNode> onAssignment(const FilePos &, const FilePos &,
                                            unique_ptr<QualifiedExpression>, unique_ptr<ExpressionNode>);
    [[nodiscard]] unique_ptr<IfThenElseNode> onIf(const FilePos &, const FilePos &,
                                                  unique_ptr<ExpressionNode>,
                                                  unique_ptr<StatementSequenceNode>,
                                                  vector<unique_ptr<ElseIfNode>>,
                                                  unique_ptr<StatementSequenceNode>) const;
    [[nodiscard]] unique_ptr<ElseIfNode> onElseIf(const FilePos &, const FilePos &,
                                                  unique_ptr<ExpressionNode>,
                                                  unique_ptr<StatementSequenceNode>) const;

    void onLoopStart(const FilePos &);
    unique_ptr<LoopNode> onLoop(const FilePos &, const FilePos &,
                                unique_ptr<StatementSequenceNode>);
    unique_ptr<WhileLoopNode> onWhileLoop(const FilePos &, const FilePos &,
                                          unique_ptr<ExpressionNode>,
                                          unique_ptr<StatementSequenceNode>,
                                          vector<unique_ptr<ElseIfNode>>);
    unique_ptr<RepeatLoopNode> onRepeatLoop(const FilePos &, const FilePos &,
                                            unique_ptr<ExpressionNode>,
                                            unique_ptr<StatementSequenceNode>);
    unique_ptr<ForLoopNode> onForLoop(const FilePos &, const FilePos &,
                                      unique_ptr<QualIdent>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<StatementSequenceNode>);
    void onCaseOfStart(const FilePos &, const FilePos &,
                       const unique_ptr<ExpressionNode> &);
    unique_ptr<CaseOfNode> onCaseOfEnd(const FilePos &, const FilePos &,
                                       unique_ptr<ExpressionNode>,
                                       vector<unique_ptr<CaseNode>>,
                                       unique_ptr<StatementSequenceNode>);
    unique_ptr<CaseLabelNode> onCaseLabel(const FilePos &, const FilePos &,
                                          const unique_ptr<ExpressionNode> &,
                                          vector<unique_ptr<ExpressionNode>>);
    unique_ptr<CaseNode> onCase(const FilePos &, const FilePos &,
                                unique_ptr<ExpressionNode> &,
                                unique_ptr<CaseLabelNode>,
                                unique_ptr<StatementSequenceNode>);
    unique_ptr<ReturnNode> onReturn(const FilePos &, const FilePos &, unique_ptr<ExpressionNode>);
    [[nodiscard]] unique_ptr<ExitNode> onExit(const FilePos &, const FilePos &) const;

    unique_ptr<StatementNode> onQualifiedStatement(const FilePos &, const FilePos &,
                                                   unique_ptr<QualIdent>, vector<unique_ptr<Selector>>);
    unique_ptr<QualifiedExpression> onQualifiedExpression(const FilePos &, const FilePos &,
                                                          unique_ptr<QualIdent>, vector<unique_ptr<Selector>>);
    unique_ptr<ExpressionNode> onQualifiedConstant(const FilePos &, const FilePos &,
                                                   const unique_ptr<QualIdent> &, const vector<unique_ptr<Selector>> &);

    unique_ptr<ExpressionNode> onUnaryExpression(const FilePos &, const FilePos &,
                                                 OperatorType,
                                                 unique_ptr<ExpressionNode>);
    unique_ptr<ExpressionNode> onBinaryExpression(const FilePos &, const FilePos &,
                                                  OperatorType,
                                                  unique_ptr<ExpressionNode>,
                                                  unique_ptr<ExpressionNode>);
    unique_ptr<ExpressionNode> onRangeExpression(const FilePos &, const FilePos &,
                                                 unique_ptr<ExpressionNode>,
                                                 unique_ptr<ExpressionNode>);
    unique_ptr<ExpressionNode> onSetExpression(const FilePos &, const FilePos &,
                                               vector<unique_ptr<ExpressionNode>>);

    unique_ptr<BooleanLiteralNode> onBooleanLiteral(const FilePos &, const FilePos &, bool);
    [[nodiscard]] unique_ptr<IntegerLiteralNode> onIntegerLiteral(const FilePos &, const FilePos &, int64_t, TypeKind = TypeKind::INTEGER) const;
    [[nodiscard]] unique_ptr<RealLiteralNode> onRealLiteral(const FilePos &, const FilePos &, double, TypeKind = TypeKind::REAL) const;
    unique_ptr<StringLiteralNode> onStringLiteral(const FilePos &, const FilePos &, const string &);
    unique_ptr<CharLiteralNode> onCharLiteral(const FilePos &, const FilePos &, uint8_t);
    [[nodiscard]] unique_ptr<NilLiteralNode> onNilLiteral(const FilePos &, const FilePos &) const;
    unique_ptr<SetLiteralNode> onSetLiteral(const FilePos &, const FilePos &, bitset<32>);

    bool isDefined(Ident *) const;
    bool isConstant(QualIdent *) const;
    bool isType(QualIdent *) const;
    bool isVariable(QualIdent *) const;
    bool isProcedure(QualIdent *) const;

private:
    [[maybe_unused]] CompilerConfig &config_;
    ASTContext *context_;
    OberonSystem *system_;

    Logger &logger_;
    vector<pair<unique_ptr<QualIdent>, PointerTypeNode *>> forwards_;
    stack<unique_ptr<ProcedureDefinitionNode>> procs_;
    unordered_map<QualifiedExpression *, TypeNode *> caseTys_;
    stack<FilePos> loops_;
    SymbolTable *symbols_;
    SymbolImporter importer_;
    SymbolExporter exporter_;
    TypeNode *boolTy_, *byteTy_, *charTy_, *shortIntTy_, *integerTy_, *longIntTy_, *realTy_, *longRealTy_,
             *stringTy_, *setTy_, *anyTy_, *noTy_, *typeTy_;

    static bool isSameIdent(Ident *, Ident *) ;
    void assertUnique(const IdentDef *, DeclarationNode *) const;
    int64_t assertInBounds(const IntegerLiteralNode *, int64_t , int64_t) const;
    bool assertAssignable(const ExpressionNode *, string &) const;

    void cast(unique_ptr<ExpressionNode> &, TypeNode *);
    void castLiteral(unique_ptr<ExpressionNode> &, TypeNode *);
    [[nodiscard]] TypeNode* intType(int64_t) const;

    void checkExport(DeclarationNode *) const;

    /**
     * Two variables a and b with types Ta and Tb are of the same type if
     * 1. Ta and Ta are both denoted by the same type identifier, or
     * 2. Ta is declared to equal Ta in a type declaration of the form Ta = Ta, or
     * 3. a and b appear in the same identifier list in a variable, record field,
     * or formal parameter declaration and are not open arrays.
     * @return true, if Ta and Ta are the same type, false otherwise
     */
    static bool isSameType(const TypeNode *, const TypeNode *);

    /**
     * Two types Ta and Tb are equal if
     * 1. Ta and Tb are the same type, or
     * 2. Ta and Tb are open array types with equal element types, or
     * 3. Ta and Tb are procedure types whose formal parameter lists match.
     * @return true, if Ta and Ta are equal types, false otherwise
     */
    static bool isEqualType(TypeNode *, TypeNode *, string &);

    /**
     * Two formal parameter lists match if
     * 1. they have the same number of parameters, and
     * 2. they have either the same function result type or none, and
     * 3. parameters at corresponding positions have equal types, and
     * 4. parameters at corresponding positions are both either value or variable parameters.
     * @return true, if the parameter lists match, false otherwise
     */
    static bool isParameterListMatching(ProcedureTypeNode*, ProcedureTypeNode *, string &);

    bool assertCompatible(const FilePos &, TypeNode *, TypeNode *, bool = false);
    TypeNode *commonType(const FilePos &, TypeNode *, TypeNode *) const;
    static bool isArrayOfChar(const TypeNode *);
    static bool isOpenArray(const TypeNode *);

    static string format(const TypeNode *, bool = false);

    static int64_t euclidean_mod(int64_t, int64_t);
    static int64_t floor_div(int64_t, int64_t);

    template<typename L, typename T>
    optional<unique_ptr<ExpressionNode>> foldBinaryOp(const FilePos &, const FilePos &,
                                                      unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &,
                                                      optional<T>, optional<T>, function<T(T, T)>, TypeNode *);
    optional<unique_ptr<ExpressionNode>> foldBooleanOp(const FilePos &, const FilePos &, OperatorType,
                                                       unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode*);
    template<typename T, typename Cast>
    optional<unique_ptr<BooleanLiteralNode>> foldRelationOp(const FilePos &, const FilePos &,
                                                            OperatorType, unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &,
                                                            Cast, TypeNode *);
    optional<unique_ptr<ExpressionNode>> foldDivModOp(const FilePos &, const FilePos &,
                                                      OperatorType, unique_ptr<ExpressionNode> &, const unique_ptr<ExpressionNode> &, TypeNode *);
    optional<unique_ptr<ExpressionNode>> foldFDivOp(const FilePos &, const FilePos &,
                                                    unique_ptr<ExpressionNode> &, const unique_ptr<ExpressionNode> &, TypeNode *);
    template<typename L, typename T>
    optional<unique_ptr<ExpressionNode>> foldSubOp(const FilePos &, const FilePos &,
                                                   unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode *);

    template<typename L, typename T>
    optional<unique_ptr<LiteralNode<T>>> clone(const FilePos &, const FilePos &, LiteralNode<T> *);
    static optional<bool> boolean_cast(const ExpressionNode *);
    static optional<int64_t> integer_cast(const ExpressionNode *);
    static optional<uint8_t> char_cast(const ExpressionNode *);
    static optional<double> real_cast(const ExpressionNode *);
    static optional<string> string_cast(const ExpressionNode *);
    static optional<bitset<32>> set_cast(const ExpressionNode *);

    optional<unique_ptr<ExpressionNode>> fold(const FilePos &, const FilePos &, ExpressionNode *);

    [[nodiscard]] optional<unique_ptr<ExpressionNode>> fold(const FilePos &, const FilePos &,
            OperatorType, const unique_ptr<ExpressionNode> &) const;

    optional<unique_ptr<ExpressionNode>> fold(const FilePos &, const FilePos &,
            OperatorType, unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode *);

    using Selectors = vector<unique_ptr<Selector>>;
    using SelectorIterator = Selectors::iterator;
    SelectorIterator &handleMissingParameters(const FilePos &, const FilePos &,
                                              TypeNode*, Selectors &, SelectorIterator &) const;
    void handleRepeatedIndices(const FilePos &, const FilePos &, Selectors &) const;
    TypeNode *onSelectors(const FilePos &, const FilePos &, DeclarationNode *, TypeNode*, Selectors &);
    TypeNode *onActualParameters(DeclarationNode *, TypeNode *, ActualParameters *);
    TypeNode *onArrayIndex(TypeNode *, ArrayIndex *) const;
    TypeNode *onDereference(TypeNode *, const Dereference *) const;
    FieldNode *onRecordField(TypeNode *, RecordField *) const;
    TypeNode *onTypeguard(DeclarationNode *, TypeNode *, Typeguard *) const;

};


#endif //OBERON_LANG_SEMA_H
