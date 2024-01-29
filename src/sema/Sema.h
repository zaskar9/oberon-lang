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

    TypeNode *resolveType(TypeNode *);

public:
    Sema(ASTContext *context, SymbolTable *symbols, Logger *logger);

//    void onCompilationBegin();
//    void onCompilationEnd();
//
//    unique_ptr<ModuleNode> onModule(FilePos start, FilePos end,
//                                    unique_ptr<Ident> ident,
//                                    vector<unique_ptr<ImportNode>> imports,
//                                    vector<unique_ptr<ConstantDeclarationNode> consts,
//                                    vector<unique_ptr<TypeDeclarationNode>> types,
//                                    vector<unique_ptr<VariableDeclarationNode>> vars,
//                                    vector<unique_ptr<ProcedureNode>> procs,
//                                    unique_ptr<StatementSequenceNode> stmts);
//
//    unique_ptr<ImportNode> onImport(FilePos start, FilePos end, unique_ptr<Ident> ident, unique_ptr<Ident> alias);
//
//    unique_ptr<ConstantDeclarationNode> onConstant(FilePos start, FilePos end, unique_ptr<IdentDef> ident,
//                                                   unique_ptr<ExpressionNode> expr);
//
//    unique_ptr<TypeDeclarationNode> onType(FilePos start, FilePos end, unique_ptr<IdentDef> ident, TypeNode* ref);
//    TypeNode* onQualifiedType(string qualifier, string ident);
    ArrayTypeNode* onArrayType(const FilePos&, const FilePos&, unique_ptr<ExpressionNode>, TypeNode*);
//    RecordTypeNode* onRecordType(FilePos start, FilePos end, vector<unique_ptr<FieldNode> fields);
//    unique_ptr<FieldNode> onRecordField(FilePos start, FilePos end, unique_ptr<IdentDef>, TypeNode* ref, int index);
//    PointerTypeNode* onPointerType(FilePos start, FilePos end, TypeNode* ref);
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

    unique_ptr<ExpressionNode> onUnaryExpression(const FilePos&, const FilePos&,
                                                 OperatorType,
                                                 unique_ptr<ExpressionNode>);
    unique_ptr<ExpressionNode> onBinaryExpression(const FilePos&, const FilePos&,
                                                  OperatorType,
                                                  unique_ptr<ExpressionNode>,
                                                  unique_ptr<ExpressionNode>);

    unique_ptr<BooleanLiteralNode> onBooleanLiteral(const FilePos&, const FilePos&, bool);
    unique_ptr<IntegerLiteralNode> onIntegerLiteral(const FilePos&, const FilePos&, long, bool = false);
    unique_ptr<RealLiteralNode> onRealLiteral(const FilePos&, const FilePos&, double, bool = false);
    unique_ptr<StringLiteralNode> onStringLiteral(const FilePos&, const FilePos&, const string&);
    unique_ptr<NilLiteralNode> onNilLiteral(const FilePos&, const FilePos&);

};


#endif //OBERON_LANG_SEMA_H
