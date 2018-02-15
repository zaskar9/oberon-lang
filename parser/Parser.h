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
#include "ast/ExpressionNode.h"
#include "symbol/Table.h"
#include "symbol/TypeSymbol.h"
#include "symbol/ProcedureSymbol.h"
#include "symbol/ArrayTypeSymbol.h"
#include "symbol/RecordTypeSymbol.h"

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
    void type_declarations();
    const ASTNode* var_declarations();
    const ASTNode* procedure_declaration();
    const std::shared_ptr<const ExpressionNode> expression();
    const std::shared_ptr<const ExpressionNode> simple_expression();
    const std::shared_ptr<const ExpressionNode> term();
    const std::shared_ptr<const ExpressionNode> factor();
    const std::shared_ptr<const TypeSymbol> type();
    const std::shared_ptr<const ArrayTypeSymbol> array_type();
    const std::shared_ptr<const RecordTypeSymbol> record_type();
    void field_list(const std::shared_ptr<RecordTypeSymbol> &rts);
    void ident_list(std::list<std::string> &idents);
    const std::shared_ptr<const ProcedureSymbol> procedure_heading();
    const ASTNode* procedure_body();
    void formal_parameters(const std::shared_ptr<ProcedureSymbol> &ps);
    void fp_section(const std::shared_ptr<ProcedureSymbol> &ps);
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

static OperatorType token_to_operator(TokenType token);

#endif //OBERON0C_PARSER_H
