//
// Created by Michael Grossniklaus on 12/17/23.
//

#ifndef OBERON_LANG_SEMA_H
#define OBERON_LANG_SEMA_H


#include <bitset>
#include <functional>
#include <unordered_map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "data/ast/ASTContext.h"
#include "data/ast/ModuleNode.h"
#include "data/ast/CaseOfNode.h"
#include "data/ast/IfThenElseNode.h"
#include "data/ast/LoopNode.h"
#include "data/symtab/SymbolTable.h"
#include "data/symtab/SymbolExporter.h"
#include "data/symtab/SymbolImporter.h"
#include "logging/Logger.h"
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

private:
    CompilerConfig &config_;
    ASTContext *context_;
    OberonSystem *system_;

    Logger &logger_;
    vector<pair<unique_ptr<QualIdent>, PointerTypeNode *>> forwards_;
    stack<unique_ptr<ProcedureNode>> procs_;
    unordered_map<QualifiedExpression *, TypeNode *> caseTys_;
    SymbolTable *symbols_;
    SymbolImporter importer_;
    SymbolExporter exporter_;
    TypeNode *boolTy_, *byteTy_, *charTy_, *shortIntTy_, *integerTy_, *longIntTy_, *realTy_, *longRealTy_,
             *stringTy_, *setTy_, *noTy_, *typeTy_;

    bool assertEqual(Ident *, Ident *) const;
    void assertUnique(IdentDef *, DeclarationNode *);
    int64_t assertInBounds(const IntegerLiteralNode *, int64_t , int64_t);
    bool assertAssignable(const ExpressionNode *, string &) const;

    static void cast(ExpressionNode *, TypeNode *);
    void castLiteral(unique_ptr<ExpressionNode> &, TypeNode *);
    TypeNode* intType(int64_t);

    void checkExport(DeclarationNode *);

    bool assertCompatible(const FilePos &, TypeNode *, TypeNode *, bool = false);
    TypeNode *commonType(const FilePos &, TypeNode *, TypeNode *) const;

    static string format(const TypeNode *, bool = false);

    static int64_t euclidean_mod(int64_t, int64_t);
    static int64_t floor_div(int64_t, int64_t);

    template<typename L, typename T>
    optional<unique_ptr<ExpressionNode>> foldBinaryOp(const FilePos &, const FilePos &,
                                                      unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &,
                                                      optional<T>, optional<T>, function<T(T, T)>, TypeNode *);
    optional<unique_ptr<ExpressionNode>> foldBooleanOp(const FilePos &, const FilePos &, OperatorType,
                                                       unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode*);
    template<typename T>
    optional<unique_ptr<BooleanLiteralNode>> foldRelationOp(const FilePos &, const FilePos &,
                                                            OperatorType, unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode *);
    optional<unique_ptr<ExpressionNode>> foldDivModOp(const FilePos &, const FilePos &,
                                                      OperatorType, unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode *);
    optional<unique_ptr<ExpressionNode>> foldFDivOp(const FilePos &, const FilePos &,
                                                    unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode *);
    template<typename L, typename T>
    optional<unique_ptr<ExpressionNode>> foldSubOp(const FilePos &, const FilePos &,
                                                   unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode *);

    template<typename L, typename T>
    optional<unique_ptr<LiteralNode<T>>> clone(const FilePos &, const FilePos &, LiteralNode<T> *);
    optional<bool> boolean_cast(const ExpressionNode *);
    optional<int64_t> integer_cast(const ExpressionNode *);
    optional<uint8_t> char_cast(const ExpressionNode *);
    optional<double> real_cast(const ExpressionNode *);
    optional<string> string_cast(const ExpressionNode *);
    optional<bitset<32>> set_cast(const ExpressionNode *);

    optional<unique_ptr<ExpressionNode>> fold(const FilePos &, const FilePos &, ExpressionNode *);

    optional<unique_ptr<ExpressionNode>> fold(const FilePos &, const FilePos &,
            OperatorType, unique_ptr<ExpressionNode> &);

    optional<unique_ptr<ExpressionNode>> fold(const FilePos &, const FilePos &,
            OperatorType, unique_ptr<ExpressionNode> &, unique_ptr<ExpressionNode> &, TypeNode *);

    void onBlockStart();
    void onBlockEnd();

    using Selectors = vector<unique_ptr<Selector>>;
    using SelectorIterator = Selectors::iterator;
    SelectorIterator &handleMissingParameters(const FilePos &, const FilePos &,
                                              TypeNode*, Selectors &, SelectorIterator &);
    void handleRepeatedIndices(const FilePos &, const FilePos &, Selectors &);
    TypeNode *onSelectors(const FilePos &, const FilePos &, DeclarationNode *, TypeNode*, Selectors &);
    TypeNode *onActualParameters(DeclarationNode *, TypeNode *, ActualParameters *);
    TypeNode *onArrayIndex(TypeNode *, ArrayIndex *);
    TypeNode *onDereference(TypeNode *, Dereference *);
    FieldNode *onRecordField(TypeNode *, RecordField *);
    TypeNode *onTypeguard(DeclarationNode *, TypeNode *, Typeguard *);

public:
    Sema(CompilerConfig &, ASTContext *, OberonSystem *);
    Sema(const Sema&) = delete;
    Sema& operator=(const Sema&) = delete;

    void onTranslationUnitStart(const string &);
    void onTranslationUnitEnd(const string &);

    unique_ptr<ModuleNode> onModuleStart(const FilePos &, unique_ptr<Ident>);
    void onModuleEnd(const FilePos&, unique_ptr<Ident>);

    unique_ptr<ImportNode> onImport(const FilePos &, const FilePos &, unique_ptr<Ident>, unique_ptr<Ident>);

    unique_ptr<ConstantDeclarationNode> onConstant(const FilePos &, const FilePos &,
                                                   unique_ptr<IdentDef>, unique_ptr<ExpressionNode>);

    unique_ptr<TypeDeclarationNode> onType(const FilePos &, const FilePos &,
                                           unique_ptr<IdentDef>, TypeNode *);
    ArrayTypeNode *onArrayType(const FilePos &, const FilePos &, vector<unique_ptr<ExpressionNode>>, TypeNode *);
    PointerTypeNode *onPointerType(const FilePos &, const FilePos &, unique_ptr<QualIdent>);
    PointerTypeNode *onPointerType(const FilePos &, const FilePos &, TypeNode *);
    ProcedureTypeNode *onProcedureType(const FilePos &, const FilePos &,
                                       vector<unique_ptr<ParameterNode>>, bool varargs, TypeNode *);
    unique_ptr<ParameterNode> onParameter(const FilePos &, const FilePos &,
                                          unique_ptr<Ident>, TypeNode *, bool, unsigned = 0);
    RecordTypeNode *onRecordType(const FilePos &, const FilePos &, unique_ptr<QualIdent>, vector<unique_ptr<FieldNode>>);
    unique_ptr<FieldNode> onField(const FilePos&, const FilePos&, unique_ptr<IdentDef>, TypeNode*, unsigned = 0);

    TypeNode *onTypeReference(const FilePos &, const FilePos &, unique_ptr<QualIdent>, unsigned = 0);

    unique_ptr<VariableDeclarationNode> onVariable(const FilePos &, const FilePos &,
                                                   unique_ptr<IdentDef>, TypeNode*, int = 0);

    void onDeclarations();

    ProcedureNode *onProcedureStart(const FilePos &, unique_ptr<IdentDef>);
    unique_ptr<ProcedureNode> onProcedureEnd(const FilePos &, unique_ptr<Ident>);

    unique_ptr<AssignmentNode> onAssignment(const FilePos &, const FilePos &,
                                            unique_ptr<QualifiedExpression>, unique_ptr<ExpressionNode>);
    unique_ptr<IfThenElseNode> onIf(const FilePos &, const FilePos &,
                                    unique_ptr<ExpressionNode>,
                                    unique_ptr<StatementSequenceNode>,
                                    vector<unique_ptr<ElseIfNode>>,
                                    unique_ptr<StatementSequenceNode>);
    unique_ptr<ElseIfNode> onElseIf(const FilePos &, const FilePos &,
                                    unique_ptr<ExpressionNode>,
                                    unique_ptr<StatementSequenceNode>);
    unique_ptr<LoopNode> onLoop(const FilePos &, const FilePos &,
                                unique_ptr<StatementSequenceNode>);
    unique_ptr<RepeatLoopNode> onRepeatLoop(const FilePos &, const FilePos &,
                                            unique_ptr<ExpressionNode>,
                                            unique_ptr<StatementSequenceNode>);
    unique_ptr<WhileLoopNode> onWhileLoop(const FilePos &, const FilePos &,
                                          unique_ptr<ExpressionNode>,
                                          unique_ptr<StatementSequenceNode>);
    unique_ptr<ForLoopNode> onForLoop(const FilePos &, const FilePos &,
                                      unique_ptr<QualIdent>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<StatementSequenceNode>);
    void onCasePfStart(const FilePos &, const FilePos &,
                       unique_ptr<ExpressionNode> &);
    unique_ptr<CaseOfNode> onCaseOfEnd(const FilePos &, const FilePos &,
                                       unique_ptr<ExpressionNode>,
                                       vector<unique_ptr<CaseNode>>,
                                       unique_ptr<StatementSequenceNode>);
    unique_ptr<CaseLabelNode> onCaseLabel(const FilePos &, const FilePos &,
                                          unique_ptr<ExpressionNode> &,
                                          vector<unique_ptr<ExpressionNode>>);
    unique_ptr<CaseNode> onCase(const FilePos &, const FilePos &,
                                unique_ptr<ExpressionNode> &,
                                unique_ptr<CaseLabelNode>,
                                unique_ptr<StatementSequenceNode>);
    unique_ptr<ReturnNode> onReturn(const FilePos &, const FilePos &, unique_ptr<ExpressionNode>);

    unique_ptr<StatementNode> onQualifiedStatement(const FilePos &, const FilePos &,
                                                   unique_ptr<QualIdent>, vector<unique_ptr<Selector>>);
    unique_ptr<QualifiedExpression> onQualifiedExpression(const FilePos &, const FilePos &,
                                                          unique_ptr<QualIdent>, vector<unique_ptr<Selector>>);
    unique_ptr<ExpressionNode> onQualifiedConstant(const FilePos &, const FilePos &,
                                                   unique_ptr<QualIdent>, vector<unique_ptr<Selector>>);

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
    unique_ptr<IntegerLiteralNode> onIntegerLiteral(const FilePos &, const FilePos &, int64_t, TypeKind = TypeKind::INTEGER);
    unique_ptr<RealLiteralNode> onRealLiteral(const FilePos &, const FilePos &, double, TypeKind = TypeKind::REAL);
    unique_ptr<StringLiteralNode> onStringLiteral(const FilePos &, const FilePos &, const string &);
    unique_ptr<CharLiteralNode> onCharLiteral(const FilePos &, const FilePos &, uint8_t);
    unique_ptr<NilLiteralNode> onNilLiteral(const FilePos &, const FilePos &);
    unique_ptr<SetLiteralNode> onSetLiteral(const FilePos &, const FilePos &, bitset<32>);

    bool isDefined(Ident *);
    bool isConstant(QualIdent *);
    bool isType(QualIdent *);
    bool isVariable(QualIdent *);
    bool isProcedure(QualIdent *);

};


#endif //OBERON_LANG_SEMA_H
