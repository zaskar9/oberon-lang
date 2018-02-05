/*
 * Header file of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include "../scanner/Scanner.h"
#include "../cmake-build-debug/ast/ASTNode.h"

class Parser
{

private:
    Scanner* _sc;

    const ASTNode* module();
    const ASTNode* ident();
    const ASTNode* declarations();
    const ASTNode* const_declarations();
    const ASTNode* type_declarations();
    const ASTNode* var_declarations();
    const ASTNode* procedure_declaration();
    const ASTNode* expression();
    const ASTNode* simple_expression();
    const ASTNode* term();
    const ASTNode* factor();
    const ASTNode* type();
    const ASTNode* array_type();
    const ASTNode* record_type();
    const ASTNode* field_list();
    const ASTNode* ident_list();
    const ASTNode* procedure_heading();
    const ASTNode* procedure_body();
    const ASTNode* formal_parameters();
    const ASTNode* fp_section();
    const ASTNode* statement_sequence();
    const ASTNode* statement();
    const ASTNode* assignment();
    const ASTNode* procedure_call();
    const ASTNode* if_statement();
    const ASTNode* while_statement();
    const ASTNode* actual_parameters();
    const ASTNode* selector();
    void logError(const std::string& msg);

public:
    explicit Parser(Scanner *sc);
    ~Parser();
    const ASTNode* parse();
};


#endif //OBERON0C_PARSER_H
