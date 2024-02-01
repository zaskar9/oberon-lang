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
#include "logging/Logger.h"
#include "data/symtab/SymbolTable.h"

using std::string;
using std::unique_ptr;
using std::vector;

class Sema {

private:
    ASTContext *context_;
    SymbolTable *symbols_;
    Logger *logger_;

    TypeNode *tBoolean_, *tByte_, *tChar_, *tInteger_, *tLongInt_, *tReal_, *tLongReal_, *tString_;

    bool assertEqual(Ident *, Ident *) const;

    TypeNode *commonType(TypeNode *, TypeNode *) const;

    TypeNode *resolveType(TypeNode *);

    bool foldBoolean(const FilePos &, const FilePos &, ExpressionNode *);

    long foldInteger(const FilePos &, const FilePos &, ExpressionNode *);

    double foldReal(const FilePos &, const FilePos &, ExpressionNode *);

    string foldString(const FilePos &, const FilePos &, ExpressionNode *);

    unique_ptr<LiteralNode> fold(const FilePos &, const FilePos &,
                                 OperatorType, ExpressionNode *);

    unique_ptr<LiteralNode> fold(const FilePos &, const FilePos &,
                                 OperatorType, ExpressionNode *, ExpressionNode *, TypeNode *);

public:
    Sema(ASTContext *context, SymbolTable *symbols, Logger *logger);

//    void onCompilationBegin();
//    void onCompilationEnd();
//
//    unique_ptr<ModuleNode> onModule(const FilePos&, const FilePos&,
//                                    unique_ptr<Ident> ident,
//                                    vector<unique_ptr<ImportNode>> imports,
//                                    vector<unique_ptr<ConstantDeclarationNode> consts,
//                                    vector<unique_ptr<TypeDeclarationNode>> types,
//                                    vector<unique_ptr<VariableDeclarationNode>> vars,
//                                    vector<unique_ptr<ProcedureNode>> procs,
//                                    unique_ptr<StatementSequenceNode> stmts);
//
//    unique_ptr<ImportNode> onImport(const FilePos&, const FilePos&, unique_ptr<Ident> ident, unique_ptr<Ident> alias);
//
//    unique_ptr<ConstantDeclarationNode> onConstant(const FilePos&, const FilePos&, unique_ptr<IdentDef> ident,
//                                                   unique_ptr<ExpressionNode> expr);
//
//    unique_ptr<TypeDeclarationNode> onType(const FilePos&, const FilePos&, unique_ptr<IdentDef> ident, TypeNode* ref);

    ArrayTypeNode *onArrayType(const FilePos &, const FilePos &, Ident *, unique_ptr<ExpressionNode>, TypeNode *);

    PointerTypeNode *onPointerType(const FilePos &, const FilePos &, Ident *, TypeNode *);

    ProcedureTypeNode *onProcedureType(const FilePos &, const FilePos &,
                                       Ident *, vector<unique_ptr<ParameterNode>>, TypeNode *);

    RecordTypeNode *onRecordType(const FilePos &, const FilePos &, Ident *, vector<unique_ptr<FieldNode>>);

//    unique_ptr<FieldNode> onRecordField(const FilePos&, FilePos&, unique_ptr<IdentDef>, TypeNode*, unsigned int);

    TypeNode *onTypeReference(const FilePos &, const FilePos &, unique_ptr<QualIdent>);

//    TypeNode* onQualifiedIdent(const FilePos &, const FilePos &, unique_ptr<QualIdent>);
//
//    unique_ptr<VariableDeclarationNode> onVariable(FilePos start, FilePos end, unique_ptr<IdentDef>, TypeNode* ref, int index);
//
//    unique_ptr<ProcedureNode> onProcedure(FilePos start, FilePos end,
//                                          unique_ptr<IdentDef> ident, bool external,
//                                          vector<unique_ptr<ParameterNode> params,
//                                          TypeNode* ref,
//                                          vector<unique_ptr<ConstantDeclarationNode> consts,
//                                          vector<unique_ptr<TypeDeclarationNode>> types,
//                                          vector<unique_ptr<VariableDeclarationNode>> vars,
//                                          vector<unique_ptr<ProcedureNode>> procs,
//                                          unique_ptr<StatementSequenceNode> stmts);
//
//    unique_ptr<StatementSequenceNode> onStatementSequence();
//    unique_ptr<IfThenElseNode> onIfThenElseStatement();

    unique_ptr<ExpressionNode> onUnaryExpression(const FilePos &, const FilePos &,
                                                 OperatorType,
                                                 unique_ptr<ExpressionNode>);

    unique_ptr<ExpressionNode> onBinaryExpression(const FilePos &, const FilePos &,
                                                  OperatorType,
                                                  unique_ptr<ExpressionNode>,
                                                  unique_ptr<ExpressionNode>);


    Node *onDesignator(const FilePos &, const FilePos &, unique_ptr<Designator>);

    unique_ptr<BooleanLiteralNode> onBooleanLiteral(const FilePos &, const FilePos &, bool);

    unique_ptr<IntegerLiteralNode> onIntegerLiteral(const FilePos &, const FilePos &, long, bool = false);

    unique_ptr<RealLiteralNode> onRealLiteral(const FilePos &, const FilePos &, double, bool = false);

    unique_ptr<StringLiteralNode> onStringLiteral(const FilePos &, const FilePos &, const string &);

    unique_ptr<NilLiteralNode> onNilLiteral(const FilePos &, const FilePos &);

};


#endif //OBERON_LANG_SEMA_H
