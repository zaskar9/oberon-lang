/*
 * Parser of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


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
#include <memory>
#include <set>
#include <vector>

class Parser {

private:
    CompilerFlags *flags_;
    Logger *logger_;
    Scanner *scanner_;
    std::unique_ptr<const Token> token_;

    std::unique_ptr<Ident> ident();
    std::unique_ptr<QualIdent> qualident();
    std::unique_ptr<Designator> designator();
    std::unique_ptr<Selector> selector();
    bool maybe_typeguard();
    std::unique_ptr<IdentDef> identdef(bool checkAlphaNum = true);
    void ident_list(std::vector<std::unique_ptr<Ident>> &idents);

    std::unique_ptr<ModuleNode> module();
    void import_list(ModuleNode *module);
    void import(ModuleNode *module);
    void declarations(BlockNode *block);
    void const_declarations(BlockNode *block);
    void type_declarations(BlockNode *block);
    void var_declarations(BlockNode *block);
    void procedure_declaration(BlockNode *block);
    std::unique_ptr<ExpressionNode> expression();
    std::unique_ptr<ExpressionNode> simple_expression();
    std::unique_ptr<ExpressionNode> term();
    std::unique_ptr<ExpressionNode> factor();
    std::unique_ptr<ExpressionNode> basic_factor();
    TypeNode* type(BlockNode *block, Ident* identifier = nullptr);
    ArrayTypeNode* array_type(BlockNode *block, Ident* identifier = nullptr);
    RecordTypeNode* record_type(BlockNode *block, Ident* identifier = nullptr);
    void field_list(BlockNode *block, RecordTypeNode *record);
    PointerTypeNode* pointer_type(BlockNode *block, Ident* identifier = nullptr);
    std::unique_ptr<ProcedureNode> procedure_heading();
    void procedure_body(ProcedureNode *proc);
    void formal_parameters(ProcedureNode *proc);
    void fp_section(ProcedureNode *proc);
    void statement_sequence(StatementSequenceNode* statements);
    std::unique_ptr<StatementNode> statement();
    std::unique_ptr<StatementNode> assignment(std::unique_ptr<ValueReferenceNode> lvalue);
    std::unique_ptr<StatementNode> if_statement();
    std::unique_ptr<StatementNode> loop_statement();
    std::unique_ptr<StatementNode> while_statement();
    std::unique_ptr<StatementNode> repeat_statement();
    std::unique_ptr<StatementNode> for_statement();
    void actual_parameters(ActualParameters *params);

    bool assertToken(const Token *token, TokenType expected);
    bool assertOberonIdent(const Ident *ident);
    void moveSelectors(std::vector<std::unique_ptr<Selector>> &selectors, Designator *designator);

    void resync(std::set<TokenType> types);

public:
    explicit Parser(CompilerFlags *flags, Logger *logger, Scanner *scanner) :
            flags_(flags), logger_(logger), scanner_(scanner), token_() { };
    ~Parser() = default;

    std::unique_ptr<ModuleNode> parse();

};


#endif //OBERON0C_PARSER_H
