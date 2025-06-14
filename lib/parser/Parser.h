/*
 * Parser of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Logger.h"
#include "Scanner.h"
#include "data/ast/ASTContext.h"
#include "data/ast/Ident.h"
#include "data/ast/ExpressionNode.h"
#include "data/ast/TypeNode.h"
#include "data/ast/ArrayTypeNode.h"
#include "data/ast/RecordTypeNode.h"
#include "data/ast/ProcedureNode.h"
#include "data/ast/PointerTypeNode.h"
#include "data/ast/ModuleNode.h"
#include "data/ast/StatementNode.h"
#include "data/ast/StatementSequenceNode.h"
#include "data/ast/NodeReference.h"
#include "compiler/CompilerConfig.h"
#include "sema/Sema.h"

using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

class Parser {

public:
    explicit Parser(CompilerConfig &config, Scanner &scanner, Sema &sema) :
            config_(config), scanner_(scanner), sema_(sema), logger_(config_.logger()) {}
    ~Parser() = default;

    void parse(ASTContext *context);

private:
    CompilerConfig &config_;
    Scanner &scanner_;
    Sema &sema_;
    Logger &logger_;
    unique_ptr<const Token> token_;

    unique_ptr<Ident> ident();
    unique_ptr<QualIdent> qualident();
    unique_ptr<IdentDef> identdef(bool checkAlphaNum = true);
    void ident_list(vector<unique_ptr<IdentDef>> &);

    void module(ASTContext *context);

    void import_list(vector<unique_ptr<ImportNode>> &);
    void import(vector<unique_ptr<ImportNode>> &);

    void declarations(vector<unique_ptr<ConstantDeclarationNode>> &,
                      vector<unique_ptr<TypeDeclarationNode>> &,
                      vector<unique_ptr<VariableDeclarationNode>> &,
                      vector<unique_ptr<ProcedureNode>> &);
    void const_declarations(vector<unique_ptr<ConstantDeclarationNode>> &);
    void type_declarations(vector<unique_ptr<TypeDeclarationNode>> &);
    void var_declarations(vector<unique_ptr<VariableDeclarationNode>> &);
    void procedure(vector<unique_ptr<ProcedureNode>> &);
    void procedure_declaration(const FilePos &, vector<unique_ptr<ProcedureNode>> &);
    void procedure_definition(const FilePos &, vector<unique_ptr<ProcedureNode>> &);

    void expression_list(vector<unique_ptr<ExpressionNode>> &);
    unique_ptr<ExpressionNode> expression();
    unique_ptr<ExpressionNode> simple_expression();
    unique_ptr<ExpressionNode> term();
    unique_ptr<ExpressionNode> factor();
    unique_ptr<ExpressionNode> basic_factor();
    unique_ptr<QualIdent> designator(vector<unique_ptr<Selector>> &);
    unique_ptr<Selector> selector();
    unique_ptr<ExpressionNode> set();
    unique_ptr<ExpressionNode> range_expression();
    void range_expression_list(vector<unique_ptr<ExpressionNode>> &);

    TypeNode* type();

    ArrayTypeNode* array_type();

    RecordTypeNode* record_type();
    void field_list(vector<unique_ptr<FieldNode>> &);

    void pointer_type(PointerTypeNode*);

    ProcedureTypeNode *procedure_type();
    TypeNode *formal_parameters(vector<unique_ptr<ParameterNode>> &, bool &);
    void procedure_body(ProcedureDefinitionNode *);
    void fp_section(vector<unique_ptr<ParameterNode>> &, bool &);
    TypeNode *formal_type();

    void statement_sequence(StatementSequenceNode *);
    unique_ptr<StatementNode> statement();
    unique_ptr<StatementNode> assignment(unique_ptr<QualifiedExpression>);
    unique_ptr<StatementNode> if_statement();
    unique_ptr<StatementNode> loop_statement();
    unique_ptr<StatementNode> while_statement();
    unique_ptr<StatementNode> repeat_statement();
    unique_ptr<StatementNode> for_statement();
    unique_ptr<StatementNode> case_statement();
    void elsif_clause(vector<unique_ptr<ElseIfNode>> &, TokenType = TokenType::kw_then);

    bool assertToken(const Token *, TokenType) const;
    bool assertString(const Token *, string &) const;
    bool assertOberonIdent(const Ident *) const;

    void resync(std::set<TokenType>);
    void expect(std::set<TokenType>);

};


#endif //OBERON0C_PARSER_H
