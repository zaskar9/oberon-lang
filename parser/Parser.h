/*
 * Parser of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include <memory>
#include <vector>
#include <set>
#include "../scanner/Scanner.h"
#include "../util/Logger.h"
#include "ast/Node.h"
#include "ast/LiteralNode.h"
#include "ast/ExpressionNode.h"
#include "ast/TypeNode.h"
#include "ast/ArrayTypeNode.h"
#include "ast/RecordTypeNode.h"
#include "ast/ProcedureNode.h"
#include "ast/ModuleNode.h"
#include "ast/StatementNode.h"
#include "ast/StatementSequenceNode.h"
#include "ast/ReferenceNode.h"
#include "ast/CallNode.h"
#include "symbol/SymbolTable.h"

class Parser {

private:
    Scanner *scanner_;
    SymbolTable *symbols_;
    Logger *logger_;
    std::unique_ptr<const Token> token_;

    std::string ident();
    std::unique_ptr<ModuleNode> module();
    void declarations(BlockNode *parent);
    void const_declarations(BlockNode *parent);
    void type_declarations(BlockNode *parent);
    void var_declarations(BlockNode *parent);
    void procedure_declaration(BlockNode *parent);
    std::unique_ptr<ExpressionNode> expression(BlockNode *parent);
    std::unique_ptr<ExpressionNode> simple_expression(BlockNode *parent);
    std::unique_ptr<ExpressionNode> term(BlockNode *parent);
    std::unique_ptr<ExpressionNode> factor(BlockNode *parent);
    TypeNode* type(BlockNode *parent, std::string name = "");
    ArrayTypeNode* array_type(BlockNode *parent, std::string name = "");
    RecordTypeNode* record_type(BlockNode *parent, std::string name = "");
    void field_list(BlockNode *parent, RecordTypeNode *record);
    void ident_list(std::vector<std::string> &idents);
    std::unique_ptr<ProcedureNode> procedure_heading(BlockNode *parent);
    void procedure_body(ProcedureNode *parent);
    void formal_parameters(ProcedureNode *parent);
    void fp_section(ProcedureNode *parent);
    void statement_sequence(BlockNode *parent, StatementSequenceNode* statements);
    std::unique_ptr<StatementNode> statement(BlockNode *parent);
    std::unique_ptr<StatementNode> assignment(BlockNode *parent, std::unique_ptr<ReferenceNode> lvalue);
    void procedure_call(BlockNode *parent, CallNode *call);
    std::unique_ptr<StatementNode> if_statement(BlockNode *parent);
    std::unique_ptr<StatementNode> loop_statement(BlockNode *parent);
    std::unique_ptr<StatementNode> while_statement(BlockNode *parent);
    std::unique_ptr<StatementNode> repeat_statement(BlockNode *parent);
    std::unique_ptr<StatementNode> for_statement(BlockNode *parent);
    void actual_parameters(BlockNode *parent, CallNode *call);
    TypeNode* selector(BlockNode *parent, ReferenceNode *ref, const TypeNode *type);

    std::unique_ptr<LiteralNode> fold(const ExpressionNode *expr) const;
    int foldNumber(const ExpressionNode *expr) const;
    bool foldBoolean(const ExpressionNode *expr) const;
    std::string foldString(const ExpressionNode *expr) const;

    bool checkActualParameter(const ProcedureNode *proc, size_t num, const ExpressionNode *expr);

    void resync(std::set<TokenType> types);

public:
    explicit Parser(Scanner *scanner, SymbolTable *symbols, Logger *logger) :
            scanner_(scanner), symbols_(symbols), logger_(logger), token_() { };
    ~Parser() = default;

    std::unique_ptr<ModuleNode> parse();

};


#endif //OBERON0C_PARSER_H
