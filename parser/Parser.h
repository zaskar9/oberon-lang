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
    std::unique_ptr<SymbolTable> symbols_;

    const std::string ident();
    std::unique_ptr<ModuleNode> module();
    void declarations(BlockNode *block);
    void const_declarations(BlockNode *block);
    void type_declarations(BlockNode *block);
    void var_declarations(BlockNode *block);
    void procedure_declaration(BlockNode *block);
    std::unique_ptr<ExpressionNode> expression();
    std::unique_ptr<ExpressionNode> simple_expression();
    std::unique_ptr<ExpressionNode> term();
    std::unique_ptr<ExpressionNode> factor();
    std::unique_ptr<TypeNode> type();
    std::unique_ptr<ArrayTypeNode> array_type();
    std::unique_ptr<RecordTypeNode> record_type();
    void field_list(RecordTypeNode *rtype);
    void ident_list(std::vector<std::string> &idents);
    std::unique_ptr<ProcedureNode> procedure_heading();
    void procedure_body(ProcedureNode *proc);
    void formal_parameters(ProcedureNode *proc);
    void fp_section(ProcedureNode *proc);
    const Node* statement_sequence();
    const Node* statement();
    const Node* assignment();
    const Node* procedure_call();
    const Node* if_statement();
    const Node* while_statement();
    const Node* actual_parameters();
    const Node* selector();
    std::unique_ptr<ValueNode> fold(const ExpressionNode *expr) const;
    const int foldNumber(const ExpressionNode *expr) const;
    const bool foldBoolean(const ExpressionNode *expr) const;
    const std::string foldString(const ExpressionNode *expr) const;

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    std::unique_ptr<Node> parse();

};

static OperatorType token_to_operator(TokenType token);

static const TypeNode* multiplex_type(int num, const TypeNode* type);

#endif //OBERON0C_PARSER_H
