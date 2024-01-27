/*
 * Parser of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include <iostream>
#include "Parser.h"
#include "../scanner/IdentToken.h"
#include "../data/ast/IfThenElseNode.h"
#include "../data/ast/LoopNode.h"

static OperatorType token_to_operator(TokenType token);

std::unique_ptr<ModuleNode> Parser::parse() {
    return module();
}

// ident = letter { letter | digit } .
std::unique_ptr<Ident> Parser::ident() {
    auto token = scanner_->peek();
    if (assertToken(token, TokenType::const_ident)) {
        token_ = scanner_->next();
        auto ident = dynamic_cast<const IdentToken*>(token_.get());
        logger_->debug(to_string(*ident));
        return std::make_unique<Ident>(ident->start(), ident->value());
    }
    // [<END>, <ELSE>, <ELSIF>, <THEN>, <UNTIL>, <BY>, <DO>, <TO>, <OF>, <MOD>, <DIV>, <OR>,
    // <<=>, <<>, <=>, <#>, <>=>, <>>, <+>, <->, <*>, <&>, <:=>, <[>, <]>, <(>, <)>, <;>, <,>, <:>, <.>]
    resync({ TokenType::kw_end, TokenType::kw_else, TokenType::kw_elsif, TokenType::kw_then,
             TokenType::kw_until, TokenType::kw_by, TokenType::kw_do, TokenType::kw_to,
             TokenType::op_mod, TokenType::op_div, TokenType::op_or, TokenType::op_leq, TokenType::op_lt,
             TokenType::op_eq, TokenType::op_neq, TokenType::op_gt, TokenType::op_geq,
             TokenType::op_plus, TokenType::op_minus, TokenType::op_times, TokenType::op_and, TokenType::op_becomes,
             TokenType::rbrack, TokenType::lbrack, TokenType::rparen, TokenType::lparen,
             TokenType::semicolon, TokenType::colon, TokenType::comma, TokenType::period });
    return std::make_unique<Ident>(token->start(), to_string(TokenType::undef));
}

// qualident = [ ident "." ] ident .
std::unique_ptr<QualIdent> Parser::qualident() {
    logger_->debug("qualident");
    auto qualifier = ident();
    if (scanner_->peek()->type() == TokenType::period) {
        scanner_->next(); // skip the period
        if (assertToken(scanner_->peek(), TokenType::const_ident)) {
            auto identifier = ident();
            return std::make_unique<QualIdent>(qualifier->pos(), qualifier->name(), identifier->name());
        }
    }
    return std::make_unique<QualIdent>(qualifier->pos(), qualifier->name());
}

// identdef = ident [ "*" ] .
std::unique_ptr<IdentDef> Parser::identdef(bool checkAlphaNum) {
    logger_->debug("identdef");
    auto identifier = ident();
    if (checkAlphaNum) {
        assertOberonIdent(identifier.get());
    }
    auto exp = false;
    if (scanner_->peek()->type() == TokenType::op_times) {
        scanner_->next(); // skip the asterisk
        exp = true;
    }
    return std::make_unique<IdentDef>(identifier->pos(), identifier->name(), exp);
}

// designator = qualident { selector } .
std::unique_ptr<Designator> Parser::designator() {
    logger_->debug("designator");
    auto identifier = qualident();
    auto designator = std::make_unique<Designator>(std::move(identifier));
    auto token = scanner_->peek();
    while (token->type() == TokenType::period ||
           token->type() == TokenType::lbrack ||
           token->type() == TokenType::caret ||
           token->type() == TokenType::lparen) {
        auto sel = selector();
        if (sel) {
            designator->addSelector(std::move(sel));
        } else {
            break;
        }
        token = scanner_->peek();
    }
    return designator;
}

// TODO selector = "." ident | "[" exp_list "]" | "^" | "(" qualident ")" | actual_parameters .
// selector = "." ident | "[" expression "]" | "^" | "(" qualident ")" | actual_parameters .
std::unique_ptr<Selector> Parser::selector() {
    logger_->debug("selector");
    std::unique_ptr<Selector> sel = nullptr;
    auto token = scanner_->peek();
    if (token->type() == TokenType::period) {
        token_ = scanner_->next();
        auto pos = token_->start();
        sel = std::make_unique<RecordField>(pos, ident());
    } else if (token->type() == TokenType::lbrack) {
        auto pos = token->start();
        token_ = scanner_->next();
        auto expr = expression();
        if (expr) {
            sel = std::make_unique<ArrayIndex>(pos, std::move(expr));
        } else {
            logger_->error(token_->start(), "expression expected.");
        }
        token_ = scanner_->next();
        if (token_->type() != TokenType::rbrack) {
            logger_->error(token_->start(), "] expected, found " + to_string(token_->type()) + ".");
        }
    } else if (token->type() == TokenType::caret) {
        token_ = scanner_->next();
        sel = std::make_unique<Dereference>(token_->start());
    } else if (token->type() == TokenType::lparen) {
        if (maybe_typeguard()) {
            token_ = scanner_->next(); // skip "("
            auto ident = qualident();
            sel = std::make_unique<Typeguard>(token_->start(), std::move((ident)));
            token_ = scanner_->next(); // skip ")"
        } else {
            sel = std::make_unique<ActualParameters>(token->start());
            actual_parameters(dynamic_cast<ActualParameters *>(sel.get()));
        }
    } else {
        logger_->error(token_->start(), "selector expected.");
    }
    return sel;
}

bool Parser::maybe_typeguard() {
    logger_->debug("maybe_typeguard");
    // we need a look-ahead of 5 to check whether we have (ident) or (ident.ident)
    auto peek = scanner_->peek(true);
    if (peek->type() == TokenType::lparen) {
        peek = scanner_->peek(true);
        if (peek->type() == TokenType::const_ident) {
            peek = scanner_->peek(true);
            if (peek->type() == TokenType::rparen) {
                return true;
            } else if (peek->type() == TokenType::period) {
                peek = scanner_->peek(true);
                if (peek->type() == TokenType::const_ident) {
                    peek = scanner_->peek(true);
                    return peek->type() == TokenType::rparen;
                }
            }
        }
    }
    return false;
}

// ident_list = identdef { "," identdef } .
void Parser::ident_list(std::vector<std::unique_ptr<Ident>> &idents) {
    logger_->debug("ident_list");
    while (true) {
        auto token = scanner_->peek();
        if (token->type() == TokenType::const_ident) {
            idents.push_back(identdef());
        } else {
            logger_->error(token->start(), "identifier expected.");
            break;
        }
        token = scanner_->peek();
        if (token->type() == TokenType::comma) {
            scanner_->next(); // skip comma
        } else if (token->type() == TokenType::const_ident) {
            logger_->error(token->start(), "comma missing.");
        } else if (token->type() == TokenType::colon) {
            break;
        } else {
            logger_->error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
            // [<:>]
            resync({ TokenType::colon });
            break;
        }
    }
}

// module = "MODULE" ident ";" [ import_list ] declarations [ "BEGIN" statement_sequence ] "END" ident "." .
std::unique_ptr<ModuleNode> Parser::module() {
    logger_->debug("module");
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_module)) {
        auto module = std::make_unique<ModuleNode>(token_->start(), ident());
        token_ = scanner_->next();
        if (assertToken(token_.get(), TokenType::semicolon)) {
            if (scanner_->peek()->type() == TokenType::kw_import) {
                import_list(module.get());
            }
            declarations(module.get());
            token_ = scanner_->next();
            if (token_->type() == TokenType::kw_begin) {
                statement_sequence(module->getStatements());
                token_ = scanner_->next();
            }
            if (assertToken(token_.get(), TokenType::kw_end)) {
                auto identifier = ident();
                if (*identifier != *module->getIdentifier()) {
                    logger_->error(scanner_->peek()->start(), "module name mismatch: expected " +
                                   to_string(*module->getIdentifier()) + ", found " + to_string(*identifier) + ".");
                }
                token_ = scanner_->next();
                if (token_->type() != TokenType::period) {
                    logger_->error(token_->start(), ". expected, found " + to_string(token_->type()) + ".");
                }
            }
        }
        return module;
    }
    // [<EOF>]
    resync({ TokenType::eof });
    return nullptr;
}

// import_list = IMPORT import {"," import} ";" .
void Parser::import_list(ModuleNode *module) {
    logger_->debug("import_list");
    scanner_->next(); // skip IMPORT keyword
    while (true) {
        import(module);
        auto token = scanner_->peek();
        if (token->type() == TokenType::comma) {
            scanner_->next(); // skip comma
        } else if (token->type() == TokenType::const_ident) {
            logger_->error(token->start(), "comma missing.");
        } else if (token->type() == TokenType::semicolon) {
            scanner_->next(); // skip semicolon
            break;
        } else {
            logger_->error(token->start(), to_string(token->type()) + "unexpected.");
            // [<CONST>, <TYPE>, <VAR>, <PROCEDURE>, <BEGIN>]
            resync({ TokenType::kw_const, TokenType::kw_type, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin });
            break;
        }
    }
}

// import = ident [":=" ident] .
void Parser::import(ModuleNode *module) {
    logger_->debug("import");
    auto token = scanner_->peek();
    if (assertToken(token, TokenType::const_ident)) {
        auto identifier = ident();
        if (scanner_->peek()->type() == TokenType::op_becomes) {
            scanner_->next(); // skip := operator
            if (assertToken(scanner_->peek(), TokenType::const_ident)) {
                auto name = ident();
                module->addImport(std::make_unique<ImportNode>(identifier->pos(), std::move(identifier), std::move(name)));
            }
        } else {
            module->addImport(std::make_unique<ImportNode>(identifier->pos(), nullptr, std::move(identifier)));
        }
    }
}

// TODO declaration_sequence = [ CONST { const_declaration ";" } ]
// TODO                        [ TYPE { type_declaration ";" } ]
// TODO                        [ VAR { variable_declaration ";" } ]
// TODO                        { procedure_declaration ";" } .
// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { procedure_declaration } .
void Parser::declarations(BlockNode *block) {
    logger_->debug("declarations");
    if (scanner_->peek()->type() == TokenType::kw_const) {
        const_declarations(block);
    }
    if (scanner_->peek()->type() == TokenType::kw_type) {
        type_declarations(block);
    }
    if (scanner_->peek()->type() == TokenType::kw_var) {
        var_declarations(block);
    }
    while (scanner_->peek()->type() == TokenType::kw_procedure) {
        procedure_declaration(block);
    }
}

// TODO const_declaration = identdef "=" const_expression .
// TODO const_expression = expression .
// const_declarations = "CONST" { identdef "=" expression ";" } .
void Parser::const_declarations(BlockNode *block) {
    logger_->debug("const_declarations");
    scanner_->next(); // skip CONST keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        auto pos = scanner_->peek()->start();
        auto name = identdef();
        auto token = scanner_->next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto constant = std::make_unique<ConstantDeclarationNode>(pos, std::move(name), expression());
            block->addConstant(std::move(constant));
            token = scanner_->next();
            if (token->type() != TokenType::semicolon) {
                logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
    // [<VAR>, <TYPE>, <PROCEDURE>, <END>, <BEGIN>]
    resync({ TokenType::kw_type, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });
}

// type_declarations =  "TYPE" { ident "=" type ";" } .
void Parser::type_declarations(BlockNode *block) {
    logger_->debug("type_declarations");
    scanner_->next(); // skip TYPE keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        auto pos = scanner_->peek()->start();
        auto identifier = identdef();
        auto name = identifier->name();
        auto token = scanner_->next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto node = std::make_unique<TypeDeclarationNode>(pos, std::move(identifier), type(block, identifier.get()));
            block->addTypeDeclaration(std::move(node));
            token = scanner_->next();
            if (token->type() != TokenType::semicolon) {
                logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
    // [<PROCEDURE>, <VAR>, <END>, <BEGIN>]
    resync({ TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });
}

// TODO type = qualident | array_type | record_type | pointer_type | procedure_type.
// type = qualident | array_type | record_type | pointer_type .
TypeNode* Parser::type(BlockNode *block, Ident* identifier) {
    logger_->debug("type");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        auto referenced = qualident();
        auto node = std::make_unique<TypeReferenceNode>(token->start(), std::move(referenced));
        auto res = node.get();
        block->registerType(std::move(node));
        return res;
    } else if (token->type() == TokenType::kw_array) {
        std::unique_ptr<ArrayTypeNode> node(array_type(block, identifier));
        auto res = node.get();
        block->registerType(std::move(node));
        return res;
    } else if (token->type() == TokenType::kw_record) {
        std::unique_ptr<RecordTypeNode> node(record_type(block, identifier));
        auto res = node.get();
        block->registerType(std::move(node));
        return res;
    } else if (token->type() == TokenType::kw_pointer) {
        std::unique_ptr<PointerTypeNode> node(pointer_type(block, identifier));
        auto res = node.get();
        block->registerType(std::move(node));
        return res;
    } else {
        logger_->error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// TODO array_type = "ARRAY" expression { "," expression } "OF" type .
// array_type = "ARRAY" expression "OF" type .
ArrayTypeNode* Parser::array_type(BlockNode *block, Ident* identifier) {
    logger_->debug("array_type");
    FilePos pos = scanner_->next()->start(); // skip ARRAY keyword and get its position
    auto expr = expression();
    if (assertToken(scanner_->peek(), TokenType::kw_of)) {
        scanner_->next(); // skip OF keyword
        return new ArrayTypeNode(pos, identifier, std::move(expr), type(block));
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// TODO record_type = "RECORD" [ "(" qualident ")" ] [ field_list { ";" field_list } ] END.
// record_type = "RECORD" field_list { ";" field_list } "END" .
RecordTypeNode* Parser::record_type(BlockNode *block, Ident* identifier) {
    logger_->debug("record_type");
    FilePos pos = scanner_->next()->start(); // skip RECORD keyword and get its position
    auto node = new RecordTypeNode(pos, identifier);
    field_list(block, node);
    while (scanner_->peek()->type() == TokenType::semicolon) {
        scanner_->next();
        field_list(block, node);
    }
    if (assertToken(scanner_->peek(), TokenType::kw_end)) {
        scanner_->next();
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return node;
}

// field_list = ident_list ":" type .
void Parser::field_list(BlockNode *block, RecordTypeNode *record) {
    logger_->debug("field_list");
    std::vector<std::unique_ptr<Ident>> idents;
    auto next = scanner_->peek();
    if (next->type() == TokenType::const_ident) {
        ident_list(idents);
        if (!idents.empty()) {
            auto token = scanner_->next();
            if (assertToken(token.get(), TokenType::colon)) {
                auto node = type(block);
                int index = 0;
                for (auto &&ident: idents) {
                    record->addField(std::make_unique<FieldNode>(ident->pos(), std::move(ident), node, index++));
                }
            }
        }
    } else {
        if (next->type() == TokenType::kw_end) {
            logger_->error(next->start(), "semicolon before END.");
        } else {
            logger_->error(next->start(), "identifier expected.");
        }
    }
    // [<;>, <END>]
    resync({ TokenType::semicolon, TokenType::kw_end });
}

// pointer_type = "POINTER" "TO" type .
PointerTypeNode* Parser::pointer_type(BlockNode *block, Ident *identifier) {
    logger_->debug("pointer_type");
    FilePos pos = scanner_->next()->start(); // skip POINTER keyword and get its position
    if (assertToken(scanner_->peek(), TokenType::kw_to)) {
        scanner_->next(); // skip TO keyword
        return new PointerTypeNode(pos, identifier, type(block));
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// var_declarations = "VAR" { ident_list ":" type ";" } .
void Parser::var_declarations(BlockNode *block) {
    logger_->debug("var_declarations");
    scanner_->next(); // skip VAR keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        std::vector<std::unique_ptr<Ident>> idents;
        ident_list(idents);
        auto token = scanner_->next();
        // auto pos = token->start();
        if (assertToken(token.get(), TokenType::colon)) {
            auto node = type(block);
            int index = 0;
            for (auto &&ident : idents) {
                auto variable = std::make_unique<VariableDeclarationNode>(ident->pos(), std::move(ident), node, index++);
                block->addVariable(std::move(variable));
            }
            token = scanner_->next();
            if (token->type() != TokenType::semicolon) {
                logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
    // [<END>, <PROCEDURE>, <BEGIN>]
    resync({ TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });
}

// procedure_declaration = procedure_heading ";" ( procedure_body ident | "EXTERN" ) ";" .
void Parser::procedure_declaration(BlockNode *block) {
    logger_->debug("procedure_declaration");
    auto proc = procedure_heading();
    auto token = scanner_->peek();
    if (token->type() != TokenType::semicolon) {
        logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
    } else {
        scanner_->next();
    }
    if (scanner_->peek()->type() == TokenType::kw_extern) {
        auto external = scanner_->next(); // skip EXTERN keyword
        if (flags_->hasFlag(Flag::ENABLE_EXTERN)) {
            proc->setExtern(true);
        } else {
            logger_->error(external->start(), "external procedure support disabled [-fenable-extern].");
        }
    } else {
        assertOberonIdent(proc->getIdentifier());
        procedure_body(proc.get());
        auto identifier = ident();
        if (*identifier != *proc->getIdentifier()) {
            logger_->error(token_->start(), "procedure name mismatch: expected " +
                    to_string(*proc->getIdentifier()) + ", found " + to_string(*identifier) + ".");
        }
    }
    token = scanner_->peek();
    if (token->type() != TokenType::semicolon) {
        logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
    } else {
        scanner_->next();
    }
    block->addProcedure(std::move(proc));
    // [<PROCEDURE>, <END>, <BEGIN>]
    resync({ TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });
}

// procedure_heading = "PROCEDURE" identdef [ formal_parameters ] [ ":" type ] .
std::unique_ptr<ProcedureNode> Parser::procedure_heading() {
    logger_->debug("procedure_heading");
    auto token = scanner_->next(); // skip PROCEDURE keyword
    auto pos = token->start();
    auto identifier = identdef(false);
    auto proc = std::make_unique<ProcedureNode>(pos, std::move(identifier));
    if (scanner_->peek()->type() == TokenType::lparen) {
        formal_parameters(proc.get());
    }
    if (scanner_->peek()->type() == TokenType::colon) {
        scanner_->next(); // skip colon
        proc->setReturnType(type(proc.get()));
    }
    auto peek = scanner_->peek();
    if (peek->type() != TokenType::semicolon) {
        logger_->error(peek->start(), "unexpected token.");
        // [<;>]
        resync({ TokenType::semicolon });
    }
    return proc;
}

// TODO procedure_body = declarations [ "BEGIN" statement_sequence ] [ "RETURN" expression] "END" .
// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
void Parser::procedure_body(ProcedureNode *proc) {
    logger_->debug("procedure_body");
    declarations(proc);
    auto token = scanner_->peek();
    if (token->type() == TokenType::kw_end) {
        scanner_->next(); // skip END keyword
    } else if (assertToken(token, TokenType::kw_begin)) {
        scanner_->next(); // skip BEGIN keyword
        statement_sequence(proc->getStatements());
    } else {
        resync({TokenType::kw_end});
    }
    // [<ident>]
    resync({ TokenType::const_ident });
}

// TODO formal_parameters = "(" [ fp_section { ";" fp_section } ] ")" [":" qualident] .
// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")" .
void Parser::formal_parameters(ProcedureNode *proc) {
    logger_->debug("formal_parameters");
    auto token = scanner_->next(); // skip left parenthesis
    if (token->type() == TokenType::lparen) {
        TokenType type = scanner_->peek()->type();
        if (type == TokenType::kw_var || type == TokenType::const_ident || type == TokenType::varargs) {
            fp_section(proc);
            while (scanner_->peek()->type() == TokenType::semicolon) {
                token = scanner_->next(); // skip semicolon
                if (proc->hasVarArgs()) {
                    logger_->error(token->start(), "varargs must be last formal parameter.");
                }
                fp_section(proc);
            }
        }
        token = scanner_->next();
        if (token->type() != TokenType::rparen) {
            logger_->error(token->start(), ") expected, found " + to_string(token->type()) + ".");
        }
    }
    // [<:>, <;>]
    resync({ TokenType::colon, TokenType::semicolon });
}

// TODO fp_section = [ "VAR" ] ident { "," ident } ":" formal_type .
// TODO formal_type = { "ARRAY" "OF" } qualident.
// fp_section = ( [ "VAR" ] ident { "," ident } ":" type | "..." ) .
void Parser::fp_section(ProcedureNode *proc) {
    logger_->debug("fp_section");
    if (scanner_->peek()->type() == TokenType::varargs) {
        auto varargs = scanner_->next(); // skip varargs
        if (flags_->hasFlag(Flag::ENABLE_VARARGS)) {
            proc->setVarArgs(true);
        } else {
            logger_->error(varargs->start(), "variadic arguments support disabled [-fenable-varargs].");
        }
    } else {
        bool var = false;
        if (scanner_->peek()->type() == TokenType::kw_var) {
            scanner_->next(); // skip VAR keyword
            var = true;
        }
        std::vector<std::unique_ptr<Ident>> idents;
        while(true) {
            idents.push_back(ident());
            auto token = scanner_-> peek();
            if (token->type() == TokenType::comma) {
                scanner_->next(); // skip comma
            } else if (token->type() == TokenType::const_ident) {
                logger_->error(token->start(), "comma missing.");
            } else if (token->type() == TokenType::colon) {
                break;
            } else {
                logger_->error(token->start(), to_string(token->type()) + "unexpected.");
                break;
            }
        }
        auto token = scanner_->next();
        if (token->type() != TokenType::colon) {
            logger_->error(token->start(), ": expected, found " + to_string(token->type()) + ".");
        }
        auto node = type(proc);
        int index = 0;
        for (auto &&ident : idents) {
            proc->addFormalParameter(std::make_unique<ParameterNode>(token->start(), std::move(ident), node, var, index++));
        }
    }
    // [<;>, <)>]
    resync({ TokenType::semicolon, TokenType::rparen });
}

// statement_sequence = statement { ";" statement } .
void Parser::statement_sequence(StatementSequenceNode *statements) {
    logger_->debug("statement_sequence");
    auto token = scanner_->peek();
    // [<UNTIL>, <ELSIF>, <ELSE>, <END>]
    if (token->type() == TokenType::kw_end || token->type() == TokenType::kw_elsif ||
        token->type() == TokenType::kw_else || token->type() == TokenType::kw_until) {
        logger_->error(token->start(), "empty statement sequence.");
    } else {
        while (true) {
            statements->addStatement(statement());
            token = scanner_->peek();
            if (token->type() == TokenType::semicolon) {
                scanner_->next(); // skip semicolon
            } else if (token->type() == TokenType::const_ident || token->type() == TokenType::kw_if ||
                       token->type() == TokenType::kw_loop || token->type() == TokenType::kw_repeat ||
                       token->type() == TokenType::kw_for || token->type() == TokenType::kw_while ||
                       token->type() == TokenType::kw_exit || token->type() == TokenType::kw_return) {
                logger_->error(token->start(), "semicolon missing.");
            } else if (token->type() == TokenType::kw_end || token->type() == TokenType::kw_elsif ||
                       token->type() == TokenType::kw_else || token->type() == TokenType::kw_until) {
                break;
            } else {
                logger_->error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
                // [<;>] and [<UNTIL>, <ELSIF>, <ELSE>, <END>]
                resync({ TokenType::semicolon,
                               TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until});
            }
        }
    }
}

// statement = ( assignment | procedure_call | if_statement | case_statement
//               while_statement | repeat_statement | for_statement | loop_statement
//               with_statement | "EXIT" | "RETURN" [ expression ] ) .
std::unique_ptr<StatementNode> Parser::statement() {
    logger_->debug("statement");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        FilePos pos = token->start();
        auto designator = this->designator();
        token = scanner_->peek();
        if (token->type() == TokenType::op_becomes) {
            return assignment(std::make_unique<ValueReferenceNode>(pos, std::move(designator)));
        } else {
            return std::make_unique<ProcedureCallNode>(pos, std::move(designator));
        }
    } else if (token->type() == TokenType::kw_if) {
        return if_statement();
    } else if (token->type() == TokenType::kw_loop) {
        return loop_statement();
    } else if (token->type() == TokenType::kw_while) {
        return while_statement();
    } else if (token->type() == TokenType::kw_repeat) {
        return repeat_statement();
    } else if (token->type() == TokenType::kw_for) {
        return for_statement();
    } else if (token->type() == TokenType::kw_return) {
        token_ = scanner_->next();
        std::set follows{ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until };
        if (follows.find(scanner_->peek()->type()) != follows.end()) {
            return std::make_unique<ReturnNode>(token_->start(), nullptr);
        }
        return std::make_unique<ReturnNode>(token_->start(), expression());
    } else {
        logger_->error(token->start(), "unknown or empty statement.");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return nullptr;
}

// assignment = designator ":=" expression .
std::unique_ptr<StatementNode> Parser::assignment(std::unique_ptr<ValueReferenceNode> lvalue) {
    logger_->debug("assignment");
    scanner_->next(); // skip assign operator
    auto statement = std::make_unique<AssignmentNode>(lvalue->pos(), std::move(lvalue), expression());
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
std::unique_ptr<StatementNode> Parser::if_statement() {
    logger_->debug("if_statement");
    token_ = scanner_->next(); // skip IF keyword
    auto condition = expression();
    auto statement = std::make_unique<IfThenElseNode>(token_->start(), std::move(condition));
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_then)) {
        statement_sequence(statement->addThenStatements(token_->start()));
        token_ = scanner_->next();
        while (token_->type() == TokenType::kw_elsif) {
            condition = expression();
            token_ = scanner_->next();
            if (assertToken(token_.get(), TokenType::kw_then)) {
                statement_sequence(statement->addElseIf(token_->start(), std::move(condition)));
            }
            token_ = scanner_->next();
        }
        if (token_->type() == TokenType::kw_else) {
            statement_sequence(statement->addElseStatements(token_->start()));
            token_ = scanner_->next();
        }
        if (token_->type() != TokenType::kw_end) {
            logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// loop_statement = "LOOP" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::loop_statement() {
    logger_->debug("loop_statement");
    token_ = scanner_->next(); // skip LOOP keyword
    auto statement = std::make_unique<LoopNode>(token_->start());
    statement_sequence(statement->getStatements());
    token_ = scanner_->next();
    if (token_->type() != TokenType::kw_end) {
        logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::while_statement() {
    logger_->debug("while_statement");
    token_ = scanner_->next(); // skip WHILE keyword
    auto statement = std::make_unique<WhileLoopNode>(token_->start(), expression());
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(statement->getStatements());
        token_ = scanner_->next();
        if (token_->type() != TokenType::kw_end) {
            logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// repeat_statement = "REPEAT" statement_sequence "UNTIL" expression .
std::unique_ptr<StatementNode> Parser::repeat_statement() {
    logger_->debug("repeat_statement");
    token_ = scanner_->next(); // skip REPEAT keyword
    auto statement = std::make_unique<RepeatLoopNode>(token_->start());
    statement_sequence(statement->getStatements());
    token_ = scanner_->next();
    if (token_->type() == TokenType::kw_until) {
        statement->setCondition(expression());
    } else {
        logger_->error(token_->start(), "UNTIL expected.");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// for_statement = "FOR" ident ":=" expression "TO" expression [ "BY" const_expression ] "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::for_statement() {
    logger_->debug("for_statement");
    token_ = scanner_->next(); // skip FOR keyword
    auto start = token_->start();
    auto ident = this->ident();
    auto pos = ident->pos();
    auto counter = std::make_unique<ValueReferenceNode>(pos, std::make_unique<Designator>(std::move(ident)));
    token_ = scanner_->next();
    std::unique_ptr<ExpressionNode> low = nullptr;
    if (assertToken(token_.get(), TokenType::op_becomes)) {
        low = expression();
    }
    token_ = scanner_->next();
    std::unique_ptr<ExpressionNode> high = nullptr;
    if (assertToken(token_.get(), TokenType::kw_to)) {
        high = expression();
    }
    std::unique_ptr<ExpressionNode> step = nullptr;
    if (scanner_->peek()->type() == TokenType::kw_by) {
        scanner_->next(); // skip BY keyword
        step = expression();
    }
    auto statement = std::make_unique<ForLoopNode>(start, std::move(counter), std::move(low), std::move(high), std::move(step));
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(statement->getStatements());
    }
    token_ = scanner_->next();
    if (token_->type() != TokenType::kw_end) {
        logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// actual_parameters = "(" [ expression { "," expression } ] ")" .
void Parser::actual_parameters(ActualParameters *params) {
    logger_->debug("actual_parameters");
    token_ = scanner_->next(); // skip left parenthesis
    if (scanner_->peek()->type() == TokenType::rparen) {
        token_ = scanner_->next();
        return;
    }
    params->addActualParameter(expression());
    while (scanner_->peek()->type() == TokenType::comma) {
        token_ = scanner_->next(); // skip comma
        params->addActualParameter(expression());
    }
    token_ = scanner_->next(); // skip right parenthesis
    assertToken(token_.get(), TokenType::rparen);
}

// expression = simple_expression [ ( "=" | "#" | "<" | "<=" | ">" | ">=" ) simple_expression ] .
std::unique_ptr<ExpressionNode> Parser::expression() {
    logger_->debug("expression");
    auto result = simple_expression();
    TokenType token = scanner_->peek()->type();
    if (token == TokenType::op_eq
        || token == TokenType::op_neq
        || token == TokenType::op_lt
        || token == TokenType::op_leq
        || token == TokenType::op_gt
        || token == TokenType::op_geq) {
        token_ = scanner_->next();
        OperatorType op = token_to_operator(token_->type());
        auto rhs = simple_expression();
        result = std::make_unique<BinaryExpressionNode>(token_->start(), op, std::move(result), std::move(rhs));
    }
    if (!result) {
        // [<END>, <ELSE>, <TO>, <THEN>, <UNTIL>, <ELSIF>, <BY>, <DO>, <OF>, <)>, <]>, <,>, <;>]
        resync({TokenType::kw_end, TokenType::kw_else, TokenType::kw_to, TokenType::kw_then, TokenType::kw_until,
                TokenType::kw_elsif, TokenType::kw_by, TokenType::kw_do, TokenType::kw_of,
                TokenType::rparen, TokenType::rbrack, TokenType::comma, TokenType::semicolon});
    }
    return result;
}

// simple_expression = [ "+" | "-" ] term { ( "+" | "-" | "OR" ) term } .
std::unique_ptr<ExpressionNode> Parser::simple_expression() {
    logger_->debug("simple_expression");
    std::unique_ptr<ExpressionNode> expr;
    TokenType token = scanner_->peek()->type();
    if (token == TokenType::op_plus) {
        token_ = scanner_->next();
        expr = term();
    } else if (token == TokenType::op_minus) {
        token_ = scanner_->next();
        expr = std::make_unique<UnaryExpressionNode>(token_->start(), OperatorType::NEG, term());
    } else {
        expr = term();
    }
    token = scanner_->peek()->type();
    while (token == TokenType::op_plus
           || token == TokenType::op_minus
           || token == TokenType::op_or) {
        token_ = scanner_->next();
        OperatorType op = token_to_operator(token_->type());
        expr = std::make_unique<BinaryExpressionNode>(token_->start(), op, std::move(expr), term());
        token = scanner_->peek()->type();
    }
    return expr;
}

// term = factor { ( "*" | "/" | "DIV" | "MOD" | "&" ) factor } .
std::unique_ptr<ExpressionNode> Parser::term() {
    logger_->debug("term");
    auto expr = factor();
    TokenType token = scanner_->peek()->type();
    while (token == TokenType::op_times
           || token == TokenType::op_divide
           || token == TokenType::op_div
           || token == TokenType::op_mod
           || token == TokenType::op_and) {
        token_ = scanner_->next();
        OperatorType op = token_to_operator(token_->type());
        expr = std::make_unique<BinaryExpressionNode>(token_->start(), op, std::move(expr), factor());
        token = scanner_->peek()->type();
    }
    return expr;
}

// factor = designator | integer | real | string | "NIL" | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
std::unique_ptr<ExpressionNode> Parser::factor() {
    logger_->debug("factor");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        FilePos pos = token->start();
        auto designator = this->designator();
        return std::make_unique<ValueReferenceNode>(pos, std::move(designator));
    }
    auto tmp = scanner_->next();
    if (token->type() == TokenType::int_literal) {
        auto number = dynamic_cast<const IntLiteralToken *>(tmp.get());
        return std::make_unique<IntegerLiteralNode>(number->start(), number->value());
    } else if (token->type() == TokenType::long_literal) {
        auto number = dynamic_cast<const LongLiteralToken *>(tmp.get());
        return std::make_unique<IntegerLiteralNode>(number->start(), number->value());
    } else if (token->type() == TokenType::float_literal) {
        auto number = dynamic_cast<const FloatLiteralToken *>(tmp.get());
        return std::make_unique<RealLiteralNode>(number->start(), number->value());
    } else if (token->type() == TokenType::double_literal) {
        auto number = dynamic_cast<const DoubleLiteralToken *>(tmp.get());
        return std::make_unique<RealLiteralNode>(number->start(), number->value());
    } else if (token->type() == TokenType::string_literal) {
        auto string = dynamic_cast<const StringLiteralToken *>(tmp.get());
        return std::make_unique<StringLiteralNode>(string->start(), string->value());
    } else if (token->type() == TokenType::boolean_literal) {
        auto boolean = dynamic_cast<const BooleanLiteralToken*>(tmp.get());
        return std::make_unique<BooleanLiteralNode>(boolean->start(), boolean->value());
    } else if (token->type() == TokenType::kw_nil) {
        return std::make_unique<NilLiteralNode>(tmp->start());
    } else if (token->type() == TokenType::lparen) {
        auto expr = expression();
        token = scanner_->peek();
        if (assertToken(token, TokenType::rparen)) {
            scanner_->next();
        }
        return expr;
    } else if (token->type() == TokenType::op_not) {
        return std::make_unique<UnaryExpressionNode>(token->start(), OperatorType::NOT, factor());
    } else {
        logger_->error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
        return nullptr;
    }
}

bool Parser::assertToken(const Token *token, TokenType expected) {
    if (token->type() == expected) {
        return true;
    }
    logger_->error(token->start(), to_string(expected) + " expected, found " + to_string(token->type()) + ".");
    return false;
}

bool Parser::assertOberonIdent(const Ident *ident) {
    if (ident->name().find('_') != std::string::npos) {
        logger_->error(ident->pos(), "illegal identifier: " + to_string(*ident) + ".");
        return false;
    }
    return true;
}

void Parser::moveSelectors(std::vector<std::unique_ptr<Selector>> &selectors, Designator *designator) {
    for (auto& selector: selectors) {
        designator->addSelector(std::move(selector));
    }
}

void Parser::resync(std::set<TokenType> types) {
    auto type = scanner_->peek()->type();
    while (type != TokenType::eof && types.find(type) == types.end()) {
        scanner_->next();
        type = scanner_->peek()->type();
    }
}

OperatorType token_to_operator(TokenType token) {
    switch(token) {
        case TokenType::op_eq:     return OperatorType::EQ;
        case TokenType::op_neq:    return OperatorType::NEQ;
        case TokenType::op_leq:    return OperatorType::LEQ;
        case TokenType::op_geq:    return OperatorType::GEQ;
        case TokenType::op_lt:     return OperatorType::LT;
        case TokenType::op_gt:     return OperatorType::GT;
        case TokenType::op_times:  return OperatorType::TIMES;
        case TokenType::op_divide: return OperatorType::DIVIDE;
        case TokenType::op_div:    return OperatorType::DIV;
        case TokenType::op_mod:    return OperatorType::MOD;
        case TokenType::op_plus:   return OperatorType::PLUS;
        case TokenType::op_minus:  return OperatorType::MINUS;
        case TokenType::op_and:    return OperatorType::AND;
        case TokenType::op_or:     return OperatorType::OR;
        case TokenType::op_not:    return OperatorType::NOT;
        default:
            std::cerr << "Cannot map token type to operator." << std::endl;
            exit(1);
    }
}
