/*
 * Header of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include <memory>
#include <vector>
#include "../scanner/Scanner.h"
#include "../util/Logger.h"
#include "ast/Node.h"
#include "ast/ValueNode.h"
#include "ast/ExpressionNode.h"
#include "ast/TypeNode.h"
#include "ast/ArrayTypeNode.h"
#include "ast/RecordTypeNode.h"
#include "ast/ProcedureNode.h"
#include "ast/ModuleNode.h"
#include "symbol/SymbolTable.h"

class Parser
{

private:
    Scanner *scanner_;
    Logger *logger_;

    const std::string ident();
    const ModuleNode* module(SymbolTable *symbols);
    void declarations(SymbolTable *symbols, BlockNode *block);
    void const_declarations(SymbolTable *symbols, BlockNode *block);
    void type_declarations(SymbolTable *symbols, BlockNode *block);
    void var_declarations(SymbolTable *symbols, BlockNode *block);
    void procedure_declaration(SymbolTable *symbols, BlockNode *block);
    std::unique_ptr<const ExpressionNode> expression();
    std::unique_ptr<const ExpressionNode> simple_expression();
    std::unique_ptr<const ExpressionNode> term();
    std::unique_ptr<const ExpressionNode> factor();
    std::unique_ptr<const TypeNode> type();
    std::unique_ptr<const ArrayTypeNode> array_type();
    std::unique_ptr<const RecordTypeNode> record_type();
    void field_list(RecordTypeNode *rtype);
    void ident_list(std::vector<std::string> &idents);
    std::unique_ptr<ProcedureNode> procedure_heading();
    const Node* procedure_body();
    void formal_parameters(ProcedureNode *ps);
    void fp_section(ProcedureNode *ps);
    const Node* statement_sequence();
    const Node* statement();
    const Node* assignment();
    const Node* procedure_call();
    const Node* if_statement();
    const Node* while_statement();
    const Node* actual_parameters();
    const Node* selector();
    std::unique_ptr<const ValueNode> fold(const ExpressionNode *expr) const;
    const int foldNumber(const ExpressionNode *expr) const;
    const bool foldBoolean(const ExpressionNode *expr) const;
    const std::string foldString(const ExpressionNode *expr) const;

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<const Node> parse();

};

static OperatorType token_to_operator(TokenType token);

static const TypeNode* multiplex_type(int num, const TypeNode* type);

#endif //OBERON0C_PARSER_H
