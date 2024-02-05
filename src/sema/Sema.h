//
// Created by Michael Grossniklaus on 12/17/23.
//

#ifndef OBERON_LANG_SEMA_H
#define OBERON_LANG_SEMA_H


#include <memory>
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

using std::string;
using std::unique_ptr;
using std::vector;

class Sema {

private:
    ASTContext *context_;
    SymbolTable *symbols_;
    SymbolImporter *importer_;
    SymbolExporter *exporter_;
    Logger *logger_;

    TypeNode *tBoolean_, *tByte_, *tChar_, *tInteger_, *tLongInt_, *tReal_, *tLongReal_, *tString_;

    bool assertEqual(Ident *, Ident *) const;

    void assertUnique(Ident *, Node *);

    void call(ProcedureNodeReference *);

    void checkExport(DeclarationNode *);

    bool assertCompatible(const FilePos &, TypeNode *, TypeNode *, bool = false);

    TypeNode *commonType(TypeNode *, TypeNode *) const;

    TypeNode *resolveType(TypeNode *);

    string format(const TypeNode *, bool = false) const;

    bool foldBoolean(const FilePos &, const FilePos &, ExpressionNode *);

    long foldInteger(const FilePos &, const FilePos &, ExpressionNode *);

    double foldReal(const FilePos &, const FilePos &, ExpressionNode *);

    string foldString(const FilePos &, const FilePos &, ExpressionNode *);

    unique_ptr<LiteralNode> fold(const FilePos &, const FilePos &,
                                 OperatorType, ExpressionNode *);

    unique_ptr<LiteralNode> fold(const FilePos &, const FilePos &,
                                 OperatorType, ExpressionNode *, ExpressionNode *, TypeNode *);

public:
    Sema(ASTContext *, SymbolTable *, SymbolImporter *, SymbolExporter *, Logger *);

    void onTranslationUnitStart(const string &);

    void onTranslationUnitEnd(const string &);

    unique_ptr<ModuleNode> onModule(const FilePos &, const FilePos &,
                                    unique_ptr<Ident>,
                                    vector<unique_ptr<ImportNode>>,
                                    vector<unique_ptr<ConstantDeclarationNode>>,
                                    vector<unique_ptr<TypeDeclarationNode>>,
                                    vector<unique_ptr<VariableDeclarationNode>>,
                                    vector<unique_ptr<ProcedureNode>>,
                                    unique_ptr<StatementSequenceNode>);

//    unique_ptr<ImportNode> onImport(const FilePos&, const FilePos&, unique_ptr<Ident> ident, unique_ptr<Ident> alias);

    unique_ptr<ConstantDeclarationNode> onConstant(const FilePos &, const FilePos &,
                                                   unique_ptr<IdentDef>, unique_ptr<ExpressionNode>);

    ArrayTypeNode *onArrayType(const FilePos &, const FilePos &, Ident *, unique_ptr<ExpressionNode>, TypeNode *);

    PointerTypeNode *onPointerType(const FilePos &, const FilePos &, Ident *, TypeNode *);

    ProcedureTypeNode *onProcedureType(const FilePos &, const FilePos &,
                                       Ident *, vector<unique_ptr<ParameterNode>>, TypeNode *);

    RecordTypeNode *onRecordType(const FilePos &, const FilePos &, Ident *, vector<unique_ptr<FieldNode>>);

//    unique_ptr<FieldNode> onRecordField(const FilePos&, FilePos&, unique_ptr<IdentDef>, TypeNode*, unsigned int);

    TypeNode *onTypeReference(const FilePos &, const FilePos &, unique_ptr<QualIdent>);

//    unique_ptr<VariableDeclarationNode> onVariable(FilePos start, FilePos end, unique_ptr<IdentDef>, TypeNode* ref, int index);

    unique_ptr<ProcedureNode> onProcedure(const FilePos &, const FilePos &,
                                          unique_ptr<IdentDef>,
                                          ProcedureTypeNode *,
                                          vector<unique_ptr<ConstantDeclarationNode>>,
                                          vector<unique_ptr<TypeDeclarationNode>>,
                                          vector<unique_ptr<VariableDeclarationNode>>,
                                          vector<unique_ptr<ProcedureNode>>,
                                          unique_ptr<StatementSequenceNode>);

//    unique_ptr<StatementSequenceNode> onStatementSequence();

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
                                      unique_ptr<Ident>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<ExpressionNode>,
                                      unique_ptr<StatementSequenceNode>);

    unique_ptr<ProcedureCallNode> onProcedureCall(const FilePos &, const FilePos &, unique_ptr<Designator>);

    unique_ptr<ExpressionNode> onUnaryExpression(const FilePos &, const FilePos &,
                                                 OperatorType,
                                                 unique_ptr<ExpressionNode>);

    unique_ptr<ExpressionNode> onBinaryExpression(const FilePos &, const FilePos &,
                                                  OperatorType,
                                                  unique_ptr<ExpressionNode>,
                                                  unique_ptr<ExpressionNode>);

    unique_ptr<ValueReferenceNode> onValueReference(const FilePos &, const FilePos &, unique_ptr<Designator>);

    unique_ptr<BooleanLiteralNode> onBooleanLiteral(const FilePos &, const FilePos &, bool);

    unique_ptr<IntegerLiteralNode> onIntegerLiteral(const FilePos &, const FilePos &, long, bool = false);

    unique_ptr<RealLiteralNode> onRealLiteral(const FilePos &, const FilePos &, double, bool = false);

    unique_ptr<StringLiteralNode> onStringLiteral(const FilePos &, const FilePos &, const string &);

    unique_ptr<NilLiteralNode> onNilLiteral(const FilePos &, const FilePos &);

};


#endif //OBERON_LANG_SEMA_H
