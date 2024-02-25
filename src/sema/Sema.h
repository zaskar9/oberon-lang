//
// Created by Michael Grossniklaus on 12/17/23.
//

#ifndef OBERON_LANG_SEMA_H
#define OBERON_LANG_SEMA_H


#include <bitset>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "data/ast/ASTContext.h"
#include "data/ast/ModuleNode.h"
#include "data/symtab/SymbolTable.h"
#include "data/symtab/SymbolExporter.h"
#include "data/symtab/SymbolImporter.h"
#include "logging/Logger.h"
#include "data/ast/IfThenElseNode.h"
#include "data/ast/LoopNode.h"
#include "system/OberonSystem.h"

using std::bitset;
using std::map;
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
    map<string, PointerTypeNode *> forwards_;
    unique_ptr<ModuleNode> module_;
    stack<unique_ptr<ProcedureNode>> procs_;
    SymbolTable *symbols_;
    SymbolImporter importer_;
    SymbolExporter exporter_;
    TypeNode *boolTy_, *byteTy, *charTy, *integerTy_, *longIntTy_, *realTy_, *longRealTy_, *stringTy_, *setTy_, *nullTy_;

    bool assertEqual(Ident *, Ident *) const;
    void assertUnique(IdentDef *, DeclarationNode *);
    long assertInBounds(const IntegerLiteralNode *, long, long);

    static void cast(ExpressionNode *, TypeNode *) ;

    void checkExport(DeclarationNode *);

    bool assertCompatible(const FilePos &, TypeNode *, TypeNode *, bool = false, bool = false);
    TypeNode *commonType(const FilePos &, TypeNode *, TypeNode *) const;

    static string format(const TypeNode *, bool = false) ;

    bool foldBoolean(const FilePos &, const FilePos &, ExpressionNode *);
    long foldInteger(const FilePos &, const FilePos &, ExpressionNode *);
    double foldReal(const FilePos &, const FilePos &, ExpressionNode *);
    string foldString(const FilePos &, const FilePos &, ExpressionNode *);
    bitset<32> foldSet(const FilePos &, const FilePos &, ExpressionNode *);
    bitset<32> foldRange(const FilePos &, const FilePos &, ExpressionNode *);
    unique_ptr<LiteralNode> fold(const FilePos &, const FilePos &, ExpressionNode *);
    unique_ptr<LiteralNode> fold(const FilePos &, const FilePos &,
                                 OperatorType, ExpressionNode *);
    unique_ptr<LiteralNode> fold(const FilePos &, const FilePos &,
                                 OperatorType, ExpressionNode *, ExpressionNode *, TypeNode *);

    void onBlockStart();
    void onBlockEnd();

    using Selectors = vector<unique_ptr<Selector>>;
    using SelectorIterator = Selectors::iterator;
    static SelectorIterator &handleMissingParameters(TypeNode*, Selectors &, SelectorIterator &);
    TypeNode *onSelectors(TypeNode*, Selectors &);
    TypeNode *onActualParameters(TypeNode*, ActualParameters*);
    TypeNode *onArrayIndex(TypeNode*, ArrayIndex*);
    TypeNode *onDereference(TypeNode*, Dereference*);
    TypeNode *onRecordField(TypeNode*, RecordField*);
    TypeNode *onTypeguard(TypeNode*, Typeguard*);

public:
    Sema(CompilerConfig &, ASTContext *, OberonSystem *);
    Sema(const Sema&) = delete;
    Sema& operator=(const Sema&) = delete;

    void onTranslationUnitStart(const string &);
    void onTranslationUnitEnd(const string &);

    ModuleNode *onModuleStart(const FilePos &, unique_ptr<Ident>, vector<unique_ptr<ImportNode>>);
    unique_ptr<ModuleNode> onModuleEnd(const FilePos&, unique_ptr<Ident>);

    unique_ptr<ImportNode> onImport(const FilePos &, const FilePos &, unique_ptr<Ident>, unique_ptr<Ident>);

    unique_ptr<ConstantDeclarationNode> onConstant(const FilePos &, const FilePos &,
                                                   unique_ptr<IdentDef>, unique_ptr<ExpressionNode>);

    unique_ptr<TypeDeclarationNode> onType(const FilePos &, const FilePos &,
                                           unique_ptr<IdentDef>, TypeNode *);
    ArrayTypeNode *onArrayType(const FilePos &, const FilePos &, Ident *, unique_ptr<ExpressionNode>, TypeNode *);
    PointerTypeNode *onPointerType(const FilePos &, const FilePos &, Ident *, unique_ptr<QualIdent>);
    PointerTypeNode *onPointerType(const FilePos &, const FilePos &, Ident *, TypeNode *);
    ProcedureTypeNode *onProcedureType(const FilePos &, const FilePos &,
                                       Ident *, vector<unique_ptr<ParameterNode>>, bool varargs, TypeNode *);

    unique_ptr<ParameterNode> onParameter(const FilePos &, const FilePos &, unique_ptr<Ident>, TypeNode *, bool, unsigned = 0);

    RecordTypeNode *onRecordType(const FilePos &, const FilePos &, Ident *, vector<unique_ptr<FieldNode>>);
    unique_ptr<FieldNode> onField(const FilePos&, const FilePos&, unique_ptr<IdentDef>, TypeNode*, unsigned = 0);

    TypeNode *onTypeReference(const FilePos &, const FilePos &, unique_ptr<QualIdent>);

    unique_ptr<VariableDeclarationNode> onVariable(const FilePos &, const FilePos &,
                                                   unique_ptr<IdentDef>, TypeNode*, int = 0);

    ProcedureNode *onProcedureStart(const FilePos &, unique_ptr<IdentDef>);
    unique_ptr<ProcedureNode> onProcedureEnd(const FilePos &, unique_ptr<Ident>);

    unique_ptr<AssignmentNode> onAssignment(const FilePos &, const FilePos &,
                                            unique_ptr<QualifiedExpression>, unique_ptr<ExpressionNode>);
    unique_ptr<IfThenElseNode> onIfStatement(const FilePos &, const FilePos &,
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
    unique_ptr<ReturnNode> onReturn(const FilePos &, const FilePos &, unique_ptr<ExpressionNode>);

    unique_ptr<StatementNode> onQualifiedStatement(const FilePos &, const FilePos &,
                                                   unique_ptr<QualIdent>, vector<unique_ptr<Selector>>);
    unique_ptr<QualifiedExpression> onQualifiedExpression(const FilePos &, const FilePos &,
                                                          unique_ptr<QualIdent>, vector<unique_ptr<Selector>>);
    unique_ptr<LiteralNode> onQualifiedConstant(const FilePos &, const FilePos &,
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
    unique_ptr<IntegerLiteralNode> onIntegerLiteral(const FilePos &, const FilePos &, long, bool = false);
    unique_ptr<RealLiteralNode> onRealLiteral(const FilePos &, const FilePos &, double, bool = false);
    unique_ptr<StringLiteralNode> onStringLiteral(const FilePos &, const FilePos &, const string &);
    unique_ptr<NilLiteralNode> onNilLiteral(const FilePos &, const FilePos &);
    unique_ptr<SetLiteralNode> onSetLiteral(const FilePos &, const FilePos &, bitset<32>);

    bool isDefined(Ident *);
    bool isConstant(QualIdent *);
    bool isType(QualIdent *);
    bool isVariable(QualIdent *);
    bool isProcedure(QualIdent *);

};


#endif //OBERON_LANG_SEMA_H
