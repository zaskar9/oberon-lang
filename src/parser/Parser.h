/*
 * Parser of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include <memory>
#include <set>
#include <vector>

#include "scanner/Scanner.h"
#include "logging/Logger.h"
#include "data/ast/Node.h"
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
#include "data/symtab/SymbolTable.h"
#include "compiler/CompilerFlags.h"
#include "data/ast/ASTContext.h"
#include "sema/Sema.h"

using std::set;
using std::unique_ptr;
using std::vector;

class Parser {

private:
    CompilerFlags *flags_;
    Scanner *scanner_;
    Sema *sema_;
    Logger *logger_;
    unique_ptr<const Token> token_;

    unique_ptr<Ident> ident();
    unique_ptr<QualIdent> qualident();
    unique_ptr<Designator> designator();
    unique_ptr<Selector> selector();
    bool maybe_typeguard();
    unique_ptr<IdentDef> identdef(bool checkAlphaNum = true);
    void ident_list(vector<unique_ptr<Ident>> &idents);

    unique_ptr<ModuleNode> module();
    void import_list(ModuleNode *module);
    void import(ModuleNode *module);
    void declarations(BlockNode *block);
    void const_declarations(BlockNode *block);
    void type_declarations(BlockNode *block);
    void var_declarations(BlockNode *block);
    void procedure_declaration(BlockNode *block);
    unique_ptr<ExpressionNode> expression();
    unique_ptr<ExpressionNode> simple_expression();
    unique_ptr<ExpressionNode> term();
    unique_ptr<ExpressionNode> factor();
    unique_ptr<ExpressionNode> basic_factor();
    TypeNode* type(Ident* identifier = nullptr);
    ArrayTypeNode* array_type(Ident* identifier = nullptr);
    RecordTypeNode* record_type(Ident* identifier = nullptr);
    void field_list(vector<unique_ptr<FieldNode>> &fields);
    PointerTypeNode* pointer_type(Ident* identifier = nullptr);
    unique_ptr<ProcedureNode> procedure_heading();
    void procedure_body(ProcedureNode *proc);
    void formal_parameters(ProcedureNode *proc);
    void fp_section(ProcedureNode *proc);
    void statement_sequence(StatementSequenceNode* statements);
    unique_ptr<StatementNode> statement();
    unique_ptr<StatementNode> assignment(unique_ptr<ValueReferenceNode> lvalue);
    unique_ptr<StatementNode> if_statement();
    unique_ptr<StatementNode> loop_statement();
    unique_ptr<StatementNode> while_statement();
    unique_ptr<StatementNode> repeat_statement();
    unique_ptr<StatementNode> for_statement();
    void actual_parameters(ActualParameters *params);

    bool assertToken(const Token *token, TokenType expected);
    bool assertOberonIdent(const Ident *ident);
    void moveSelectors(vector<unique_ptr<Selector>> &selectors, Designator *designator);

    void resync(set<TokenType> types);

public:
    explicit Parser(CompilerFlags *flags, Scanner *scanner, Sema *sema, Logger *logger) :
            flags_(flags), scanner_(scanner), sema_(sema), logger_(logger), token_() { };
    ~Parser() = default;

    ModuleNode *parse(ASTContext *context);

};


#endif //OBERON0C_PARSER_H
