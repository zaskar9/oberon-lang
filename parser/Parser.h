/*
 * Header of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include "../scanner/Scanner.h"
#include "../util/Logger.h"
#include "ast/ASTNode.h"
#include "symbol/Table.h"
#include "symbol/TypeSymbol.h"

class Parser
{

private:
    Scanner *scanner_;
    Table *symbols_;
    Logger *logger_;

    const ASTNode* module();
    const std::string ident();
    const ASTNode* declarations();
    const ASTNode* const_declarations();
    const ASTNode* type_declarations();
    const ASTNode* var_declarations();
    const ASTNode* procedure_declaration();
    const ASTNode* expression();
    const ASTNode* simple_expression();
    const ASTNode* term();
    const ASTNode* factor();
    const TypeSymbol* type();
    const ASTNode* array_type();
    const ASTNode* record_type();
    const ASTNode* field_list();
    void ident_list(std::list<std::string> &idents);
    const ASTNode* procedure_heading();
    const ASTNode* procedure_body();
    const ASTNode* formal_parameters();
    const int fp_section(const int start);
    const ASTNode* statement_sequence();
    const ASTNode* statement();
    const ASTNode* assignment();
    const ASTNode* procedure_call();
    const ASTNode* if_statement();
    const ASTNode* while_statement();
    const ASTNode* actual_parameters();
    const ASTNode* selector();

public:
    explicit Parser(Scanner *scanner, Table *symbols, Logger *logger);
    ~Parser();
    const ASTNode* parse();

};

#endif //OBERON0C_PARSER_H
