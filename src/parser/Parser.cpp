/*
 * Parser of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include "Parser.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "../scanner/IdentToken.h"

using std::make_unique;
using std::unique_ptr;
using std::vector;

static OperatorType token_to_operator(TokenType token);

void Parser::parse(ASTContext *context) {
    context->setTranslationUnit(module());
}

// ident = letter { letter | digit } .
unique_ptr<Ident> Parser::ident() {
    auto token = scanner_->peek();
    if (assertToken(token, TokenType::const_ident)) {
        token_ = scanner_->next();
        auto ident = dynamic_cast<const IdentToken*>(token_.get());
        logger_->debug(to_string(*ident));
        return make_unique<Ident>(ident->start(), ident->end(), ident->value());
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
    return make_unique<Ident>(token->start(), token->end(), to_string(TokenType::undef));
}

// qualident = [ ident "." ] ident .
unique_ptr<QualIdent> Parser::qualident() {
    logger_->debug("qualident");
    auto qualifier = ident();
    if (scanner_->peek()->type() == TokenType::period) {
        scanner_->next(); // skip the period
        if (assertToken(scanner_->peek(), TokenType::const_ident)) {
            auto identifier = ident();
            return make_unique<QualIdent>(qualifier->start(), identifier->end(), qualifier->name(), identifier->name());
        }
    }
    return make_unique<QualIdent>(qualifier->start(), qualifier->end(), qualifier->name());
}

// identdef = ident [ "*" ] .
unique_ptr<IdentDef> Parser::identdef(bool checkAlphaNum) {
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
    return make_unique<IdentDef>(identifier->start(), identifier->end(), identifier->name(), exp);
}

// designator = qualident { selector } .
unique_ptr<Designator> Parser::designator() {
    logger_->debug("designator");
    auto identifier = qualident();
    auto designator = make_unique<Designator>(std::move(identifier));
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
unique_ptr<Selector> Parser::selector() {
    logger_->debug("selector");
    unique_ptr<Selector> sel = nullptr;
    auto token = scanner_->peek();
    if (token->type() == TokenType::period) {
        token_ = scanner_->next();
        auto pos = token_->start();
        sel = make_unique<RecordField>(pos, ident());
    } else if (token->type() == TokenType::lbrack) {
        auto pos = token->start();
        token_ = scanner_->next();
        auto expr = expression();
        if (expr) {
            sel = make_unique<ArrayIndex>(pos, std::move(expr));
        } else {
            logger_->error(token_->start(), "expression expected.");
        }
        token_ = scanner_->next();
        if (token_->type() != TokenType::rbrack) {
            logger_->error(token_->start(), "] expected, found " + to_string(token_->type()) + ".");
        }
    } else if (token->type() == TokenType::caret) {
        token_ = scanner_->next();
        sel = make_unique<Dereference>(token_->start());
    } else if (token->type() == TokenType::lparen) {
        if (maybe_typeguard()) {
            token_ = scanner_->next(); // skip "("
            auto ident = qualident();
            if (sema_->isType(ident.get())) {
                sel = make_unique<Typeguard>(token_->start(), std::move((ident)));
            } else {
                sel = make_unique<ActualParameters>(token->start());
                unique_ptr<ExpressionNode> param;
                if (sema_->isConstant(ident.get())) {
                    param = sema_->onConstantReference(ident->start(), ident->end(), make_unique<Designator>(std::move(ident)));
                } else {
                    param = sema_->onValueReference(ident->start(), ident->end(), make_unique<Designator>(std::move(ident)));
                }
                dynamic_cast<ActualParameters *>(sel.get())->addActualParameter(std::move(param));
            }
            token_ = scanner_->next(); // skip ")"
        } else {
            sel = make_unique<ActualParameters>(token->start());
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
void Parser::ident_list(vector<unique_ptr<IdentDef>> &idents) {
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
unique_ptr<ModuleNode> Parser::module() {
    logger_->debug("module");
    token_ = scanner_->next();
    if (!assertToken(token_.get(), TokenType::kw_module)) {
        resync({TokenType::eof});   // [<EOF>]
    }
    auto start = token_->start();
    auto name = ident();
    sema_->onTranslationUnitStart(to_string(*name));
    token_ = scanner_->next();
    if (!assertToken(token_.get(), TokenType::semicolon)) {
        // [<IMPORT>, <CONST>, <TYPE>, <VAR>, <PROCEDURE>, <BEGIN>, <END>]
        resync({TokenType::kw_import, TokenType::kw_const, TokenType::kw_type, TokenType::kw_var,
                TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end});
    }
    vector<unique_ptr<ImportNode>> imports;
    if (scanner_->peek()->type() == TokenType::kw_import) {
        import_list(imports);
    }
    sema_->onBlockStart();
    vector<unique_ptr<ConstantDeclarationNode>> consts;
    vector<unique_ptr<TypeDeclarationNode>> types;
    vector<unique_ptr<VariableDeclarationNode>> vars;
    vector<unique_ptr<ProcedureNode>> procs;
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    declarations(consts, types, vars, procs);
    token_ = scanner_->next();
    if (token_->type() == TokenType::kw_begin) {
        statement_sequence(stmts.get());
        token_ = scanner_->next();
    }
    if (!assertToken(token_.get(), TokenType::kw_end)) {
        resync({TokenType::eof});   // [<EOF>]
    }
    auto identifier = ident();
    if (*identifier != *name) {
        logger_->error(scanner_->peek()->start(), "module name mismatch: expected " +
                                                  to_string(*name) + ", found " + to_string(*identifier) + ".");
    }
    token_ = scanner_->next();
    if (!assertToken(token_.get(), TokenType::period)) {
        resync({TokenType::eof});   // [<EOF>]
    }
    sema_->onBlockEnd();
    auto module = sema_->onModule(start, token_->end(), std::move(name), std::move(imports), std::move(consts),
                                  std::move(types), std::move(vars), std::move(procs), std::move(stmts));
    sema_->onTranslationUnitEnd(to_string(*identifier));
    return module;
}

// import_list = IMPORT import { "," import } ";" .
void Parser::import_list(vector<unique_ptr<ImportNode>> &imports) {
    logger_->debug("import_list");
    scanner_->next(); // skip IMPORT keyword
    while (true) {
        import(imports);
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
void Parser::import(vector<unique_ptr<ImportNode>> &imports) {
    logger_->debug("import");
    auto token = scanner_->peek();
    if (assertToken(token, TokenType::const_ident)) {
        auto identifier = ident();
        if (scanner_->peek()->type() == TokenType::op_becomes) {
            scanner_->next(); // skip := operator
            if (assertToken(scanner_->peek(), TokenType::const_ident)) {
                auto name = ident();
                imports.push_back(sema_->onImport(identifier->start(), identifier->end(), std::move(identifier), std::move(name)));
            }
        } else {
            imports.push_back(sema_->onImport(identifier->start(), identifier->end(), nullptr, std::move(identifier)));
        }
    }
}

// TODO declaration_sequence = [ CONST { const_declaration ";" } ]
// TODO                        [ TYPE { type_declaration ";" } ]
// TODO                        [ VAR { variable_declaration ";" } ]
// TODO                        { procedure_declaration ";" } .
// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { procedure_declaration } .
void Parser::declarations(vector<unique_ptr<ConstantDeclarationNode>> & consts,
                          vector<unique_ptr<TypeDeclarationNode>> & types,
                          vector<unique_ptr<VariableDeclarationNode>> & vars,
                          vector<unique_ptr<ProcedureNode>> &procs) {
    logger_->debug("declarations");
    if (scanner_->peek()->type() == TokenType::kw_const) {
        const_declarations(consts);
    }
    if (scanner_->peek()->type() == TokenType::kw_type) {
        type_declarations(types);
    }
    if (scanner_->peek()->type() == TokenType::kw_var) {
        var_declarations(vars);
    }
    while (scanner_->peek()->type() == TokenType::kw_procedure) {
        procedure_declaration(procs);
    }
}

// TODO const_declaration = identdef "=" const_expression .
// TODO const_expression = expression .
// const_declarations = "CONST" { identdef "=" expression ";" } .
void Parser::const_declarations(vector<unique_ptr<ConstantDeclarationNode>> & consts) {
    logger_->debug("const_declarations");
    scanner_->next(); // skip CONST keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        auto pos = scanner_->peek()->start();
        auto ident = identdef();
        auto token = scanner_->next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto decl = sema_->onConstant(pos, EMPTY_POS, std::move(ident), expression());
            consts.push_back(std::move(decl));
            token = scanner_->next();
            if (token->type() != TokenType::semicolon) {
                logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
    // [<VAR>, <TYPE>, <PROCEDURE>, <END>, <BEGIN>]
    resync({ TokenType::kw_type, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });
}

// type_declarations =  "TYPE" { identdef "=" type ";" } .
void Parser::type_declarations(vector<unique_ptr<TypeDeclarationNode>> & types) {
    logger_->debug("type_declarations");
    scanner_->next(); // skip TYPE keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        auto pos = scanner_->peek()->start();
        auto ident = identdef();
        auto token = scanner_->next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto node = type(ident.get());
            auto decl = sema_->onType(pos, EMPTY_POS, std::move(ident), node);
            types.push_back(std::move(decl));
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
TypeNode* Parser::type(Ident* identifier) {
    logger_->debug("type");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        FilePos start = token->start();
        FilePos end = token->end();
        return sema_->onTypeReference(start, end, qualident());
    } else if (token->type() == TokenType::kw_array) {
        return array_type(identifier);
    } else if (token->type() == TokenType::kw_record) {
        return record_type(identifier);
    } else if (token->type() == TokenType::kw_pointer) {
        return pointer_type(identifier);
    } else {
        logger_->error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// TODO array_type = "ARRAY" expression { "," expression } "OF" type .
// array_type = "ARRAY" expression "OF" type .
ArrayTypeNode* Parser::array_type(Ident* identifier) {
    logger_->debug("array_type");
    FilePos pos = scanner_->next()->start(); // skip ARRAY keyword and get its position
    auto expr = expression();
    if (assertToken(scanner_->peek(), TokenType::kw_of)) {
        scanner_->next(); // skip OF keyword
        return sema_->onArrayType(pos, EMPTY_POS, identifier, std::move(expr), type());
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// TODO record_type = "RECORD" [ "(" qualident ")" ] [ field_list { ";" field_list } ] END.
// record_type = "RECORD" field_list { ";" field_list } "END" .
RecordTypeNode* Parser::record_type(Ident* identifier) {
    logger_->debug("record_type");
    FilePos pos = scanner_->next()->start(); // skip RECORD keyword and get its position
    vector<unique_ptr<FieldNode>> fields;
    field_list(fields);
    while (scanner_->peek()->type() == TokenType::semicolon) {
        scanner_->next();
        field_list(fields);
    }
    if (assertToken(scanner_->peek(), TokenType::kw_end)) {
        scanner_->next();
    }
    // [<)>, <;>, <END>]
    // resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return sema_->onRecordType(pos, EMPTY_POS, identifier, std::move(fields));
}

// field_list = ident_list ":" type .
void Parser::field_list(vector<unique_ptr<FieldNode>> &fields) {
    logger_->debug("field_list");
    vector<unique_ptr<IdentDef>> idents;
    auto next = scanner_->peek();
    if (next->type() == TokenType::const_ident) {
        ident_list(idents);
        if (!idents.empty()) {
            auto token = scanner_->next();
            if (assertToken(token.get(), TokenType::colon)) {
                auto node = type();
                unsigned index = 0;
                for (auto &&ident: idents) {
                    fields.push_back(sema_->onField(ident->start(), ident->end(), std::move(ident), node, index++));
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
PointerTypeNode* Parser::pointer_type(Ident *identifier) {
    logger_->debug("pointer_type");
    FilePos pos = scanner_->next()->start(); // skip POINTER keyword and get its position
    if (assertToken(scanner_->peek(), TokenType::kw_to)) {
        scanner_->next(); // skip TO keyword
        // handle possible forward if next token is a qualident
        if (scanner_->peek()->type() == TokenType::const_ident) {
            return sema_->onPointerType(pos, EMPTY_POS, identifier, qualident());
        }
        return sema_->onPointerType(pos, EMPTY_POS, identifier, type());
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// var_declarations = "VAR" { ident_list ":" type ";" } .
void Parser::var_declarations(vector<unique_ptr<VariableDeclarationNode>> &vars) {
    logger_->debug("var_declarations");
    scanner_->next(); // skip VAR keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        vector<unique_ptr<IdentDef>> idents;
        ident_list(idents);
        auto token = scanner_->next();
        // auto start = token->start();
        if (assertToken(token.get(), TokenType::colon)) {
            auto node = type();
            int index = 0;
            for (auto &&ident : idents) {
                auto decl = sema_->onVariable(ident->start(), ident->end(), std::move(ident), node, index++);
                vars.push_back(std::move(decl));
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

// procedure_declaration = "PROCEDURE" identdef [ procedure_signature ] ";" ( procedure_body ident | "EXTERN" ) ";" .
void Parser::procedure_declaration(vector<unique_ptr<ProcedureNode>> &procs) {
    logger_->debug("procedure_declaration");
    token_ = scanner_->next(); // skip PROCEDURE keyword
    FilePos start = token_->start();
    auto proc = sema_->onProcedureStart(start, identdef(false));
    sema_->onBlockStart();
    auto token = scanner_->peek();
    if (token->type() == TokenType::lparen) {
        proc->setType(procedure_signature(proc->getIdentifier()));
    } else {
        vector<unique_ptr<ParameterNode>> params;
        proc->setType(sema_->onProcedureType(token->start(), token->end(), proc->getIdentifier(), std::move(params), nullptr));
    }
    token = scanner_->peek();
    if (token->type() != TokenType::semicolon) {
        logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
    } else {
        scanner_->next();
    }
    bool external = false;
    if (scanner_->peek()->type() == TokenType::kw_extern) {
        auto ext = scanner_->next(); // skip EXTERN keyword
        if (flags_->hasFlag(Flag::ENABLE_EXTERN)) {
            external = true;
        } else {
            logger_->error(ext->start(), "external procedure support disabled [-fenable-extern].");
        }
    } else {
        assertOberonIdent(proc->getIdentifier());
        procedure_body(proc);
        auto identifier = ident();
        if (*identifier != *proc->getIdentifier()) {
            logger_->error(token_->start(), "procedure name mismatch: expected " + to_string(*proc->getIdentifier()) +
                                            ", found " + to_string(*identifier) + ".");
        }
    }
    token = scanner_->peek();
    if (token->type() != TokenType::semicolon) {
        logger_->error(token->start(), "; expected, found " + to_string(token->type()) + ".");
    } else {
        token_ = scanner_->next();
    }
    sema_->onBlockEnd();
    procs.push_back(sema_->onProcedureEnd(token->end(), external));
    // [<PROCEDURE>, <END>, <BEGIN>]
    resync({ TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });
}

// procedure_heading = formal_parameters [ ":" type ] .
ProcedureTypeNode* Parser::procedure_signature(Ident* ident) {
    logger_->debug("procedure_signature");
    // token_ = scanner_->next(); // skip "(" token
    FilePos start = token_->start();
    vector<unique_ptr<ParameterNode>> params;
    bool varargs = false;
    formal_parameters(params, varargs);
    TypeNode *ret = nullptr;
    if (scanner_->peek()->type() == TokenType::colon) {
        scanner_->next(); // skip colon
        ret = type();
    }
    auto peek = scanner_->peek();
    if (peek->type() != TokenType::semicolon) {
        logger_->error(peek->start(), "unexpected token.");
        // [<;>]
        resync({TokenType::semicolon});
    }
    auto type = sema_->onProcedureType(start, peek->end(), ident, std::move(params), ret);
    type->setVarArgs(varargs);
    return type;
}

// TODO procedure_body = declarations [ "BEGIN" statement_sequence ] [ "RETURN" expression] "END" .
// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
void Parser::procedure_body(ProcedureNode *proc) {
    logger_->debug("procedure_body");
    declarations(proc->constants(), proc->types(), proc->variables(), proc->procedures());
    auto token = scanner_->peek();
    if (token->type() == TokenType::kw_end) {
        scanner_->next(); // skip END keyword
    } else if (assertToken(token, TokenType::kw_begin)) {
        scanner_->next(); // skip BEGIN keyword
        statement_sequence(proc->statements());
    } else {
        resync({TokenType::kw_end});
    }
    // [<ident>]
    resync({ TokenType::const_ident });
}

// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")" .
void Parser::formal_parameters(vector<unique_ptr<ParameterNode>> &params, bool &varargs) {
    logger_->debug("formal_parameters");
    auto token = scanner_->next(); // skip left parenthesis
    if (token->type() == TokenType::lparen) {
        TokenType type = scanner_->peek()->type();
        while (type == TokenType::kw_var || type == TokenType::const_ident || type == TokenType::varargs) {
            fp_section(params, varargs);
            if (scanner_->peek()->type() == TokenType::rparen) {
                token = scanner_->next();
                break;
            }
            if (scanner_->peek()->type() == TokenType::semicolon) {
                token = scanner_->next(); // skip semicolon
                if (varargs) {
                    logger_->error(token->start(), "variadic arguments must be last formal parameter.");
                }
                type = scanner_->peek()->type();
                continue;
            }
            token = scanner_->next();
            logger_->error(token->start(), "; or ) expected, found " + to_string(token->type()) + ".");
        }
        if (type == TokenType::rparen) {
            token = scanner_->next();
        }
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
void Parser::fp_section(vector<unique_ptr<ParameterNode>> &params, bool &varargs) {
    logger_->debug("fp_section");
    if (scanner_->peek()->type() == TokenType::varargs) {
        auto token = scanner_->next(); // skip varargs
        if (flags_->hasFlag(Flag::ENABLE_VARARGS)) {
            varargs = true;
        } else {
            logger_->error(token->start(), "variadic arguments support disabled [-fenable-varargs].");
        }
    } else {
        bool var = false;
        if (scanner_->peek()->type() == TokenType::kw_var) {
            scanner_->next(); // skip VAR keyword
            var = true;
        }
        vector<unique_ptr<Ident>> idents;
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
        auto node = type();
        unsigned index = 0;
        for (auto &&ident : idents) {
            params.push_back(sema_->onParameter(ident->start(), ident->end(), std::move(ident), node, var, index++));
        }
    }
    // [<;>, <)>]
    // resync({ TokenType::semicolon, TokenType::rparen });
}

// statement_sequence = statement { ";" statement } .
void Parser::statement_sequence(StatementSequenceNode* statements) {
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
unique_ptr<StatementNode> Parser::statement() {
    logger_->debug("statement");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        FilePos pos = token->start();
        auto designator = this->designator();
        token = scanner_->peek();
        if (token->type() == TokenType::op_becomes) {
            return assignment(sema_->onValueReference(pos, EMPTY_POS, std::move(designator)));
        } else {
            return sema_->onProcedureCall(pos, EMPTY_POS, std::move(designator));
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
            return sema_->onReturn(token_->start(), token_->end(), nullptr);
        }
        return sema_->onReturn(token_->start(), token->end(), expression());
    } else {
        logger_->error(token->start(), "unknown or empty statement.");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return nullptr;
}

// assignment = designator ":=" expression .
unique_ptr<StatementNode> Parser::assignment(unique_ptr<ValueReferenceNode> lvalue) {
    logger_->debug("assignment");
    scanner_->next(); // skip assign operator
    auto statement = sema_->onAssignment(lvalue->pos(), EMPTY_POS, std::move(lvalue), expression());
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
unique_ptr<StatementNode> Parser::if_statement() {
    logger_->debug("if_statement");
    token_ = scanner_->next(); // skip IF keyword
    FilePos ifStart = token_->start();
    unique_ptr<ExpressionNode> ifCond = expression();
    auto thenStmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    vector<unique_ptr<ElseIfNode>> elseIfs;
    auto elseStmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_then)) {
        statement_sequence(thenStmts.get());
        token_ = scanner_->next();
        while (token_->type() == TokenType::kw_elsif) {
            FilePos elseIfStart = token_->start();
            auto elseIfCond = expression();
            token_ = scanner_->next();
            if (assertToken(token_.get(), TokenType::kw_then)) {
                auto elseIfStmts = make_unique<StatementSequenceNode>(EMPTY_POS);
                statement_sequence(elseIfStmts.get());
                elseIfs.push_back(sema_->onElseIf(elseIfStart, token_->end(),
                                                  std::move(elseIfCond), std::move(elseIfStmts)));
            }
            token_ = scanner_->next();
        }
        if (token_->type() == TokenType::kw_else) {
            statement_sequence(elseStmts.get());
            token_ = scanner_->next();
        }
        if (token_->type() != TokenType::kw_end) {
            logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_->onIfStatement(ifStart, token_->end(),
                                std::move(ifCond), std::move(thenStmts), std::move(elseIfs), std::move(elseStmts));
}

// loop_statement = "LOOP" statement_sequence "END" .
unique_ptr<StatementNode> Parser::loop_statement() {
    logger_->debug("loop_statement");
    token_ = scanner_->next(); // skip LOOP keyword
    FilePos start = token_->start();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    statement_sequence(stmts.get());
    token_ = scanner_->next();
    if (token_->type() != TokenType::kw_end) {
        logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_->onLoop(start, token_->end(), std::move(stmts));
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
unique_ptr<StatementNode> Parser::while_statement() {
    logger_->debug("while_statement");
    token_ = scanner_->next(); // skip WHILE keyword
    FilePos start = token_->start();
    auto cond = expression();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(stmts.get());
        token_ = scanner_->next();
        if (token_->type() != TokenType::kw_end) {
            logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_->onWhileLoop(start, token_->end(), std::move(cond), std::move(stmts));
}

// repeat_statement = "REPEAT" statement_sequence "UNTIL" expression .
unique_ptr<StatementNode> Parser::repeat_statement() {
    logger_->debug("repeat_statement");
    token_ = scanner_->next(); // skip REPEAT keyword
    FilePos start = token_->start();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    statement_sequence(stmts.get());
    unique_ptr<ExpressionNode> cond;
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_until)) {
        cond = expression();
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_->onRepeatLoop(start, token_->end(), std::move(cond), std::move(stmts));
}

// for_statement = "FOR" ident ":=" expression "TO" expression [ "BY" const_expression ] "DO" statement_sequence "END" .
unique_ptr<StatementNode> Parser::for_statement() {
    logger_->debug("for_statement");
    token_ = scanner_->next(); // skip FOR keyword
    FilePos start = token_->start();
    unique_ptr<Ident> var = ident();
    token_ = scanner_->next();
    unique_ptr<ExpressionNode> low;
    if (assertToken(token_.get(), TokenType::op_becomes)) {
        low = expression();
    }
    token_ = scanner_->next();
    unique_ptr<ExpressionNode> high;
    if (assertToken(token_.get(), TokenType::kw_to)) {
        high = expression();
    }
    unique_ptr<ExpressionNode> step;
    if (scanner_->peek()->type() == TokenType::kw_by) {
        scanner_->next(); // skip BY keyword
        step = expression();
    }
    token_ = scanner_->next();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(stmts.get());
    }
    token_ = scanner_->next();
    if (token_->type() != TokenType::kw_end) {
        logger_->error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_->onForLoop(start, token_->end(), std::move(var),
                            std::move(low), std::move(high), std::move(step),
                            std::move(stmts));
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
unique_ptr<ExpressionNode> Parser::expression() {
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
        result = sema_->onBinaryExpression(token_->start(), EMPTY_POS, op, std::move(result), std::move(rhs));
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
unique_ptr<ExpressionNode> Parser::simple_expression() {
    logger_->debug("simple_expression");
    unique_ptr<ExpressionNode> expr;
    TokenType token = scanner_->peek()->type();
    if (token == TokenType::op_plus) {
        token_ = scanner_->next();
        expr = term();
    } else if (token == TokenType::op_minus) {
        token_ = scanner_->next();
        expr = sema_->onUnaryExpression(token_->start(), EMPTY_POS, OperatorType::NEG, term());
    } else {
        expr = term();
    }
    token = scanner_->peek()->type();
    while (token == TokenType::op_plus
           || token == TokenType::op_minus
           || token == TokenType::op_or) {
        token_ = scanner_->next();
        OperatorType op = token_to_operator(token_->type());
        expr = sema_->onBinaryExpression(token_->start(), EMPTY_POS, op, std::move(expr), term());
        token = scanner_->peek()->type();
    }
    return expr;
}

// term = factor { ( "*" | "/" | "DIV" | "MOD" | "&" ) factor } .
unique_ptr<ExpressionNode> Parser::term() {
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
        expr = sema_->onBinaryExpression(token_->start(), EMPTY_POS, op, std::move(expr), factor());
        token = scanner_->peek()->type();
    }
    return expr;
}

// factor = [ "+" | "-" | "~" ] basic_factor .
unique_ptr<ExpressionNode> Parser::factor() {
    logger_->debug("factor");
    auto token = scanner_->peek();
    if (token->type() == TokenType::op_plus) {
        scanner_->next();   // skip plus sign
    } else if (token->type() == TokenType::op_minus) {
        auto op = scanner_->next();
        return sema_->onUnaryExpression(op->start(), EMPTY_POS, OperatorType::NEG, basic_factor());
    } else if (token->type() == TokenType::op_not) {
        auto op = scanner_->next();
        return sema_->onUnaryExpression(op->start(), EMPTY_POS, OperatorType::NOT, basic_factor());
    }
    return basic_factor();
}

// basic_factor = designator | integer | real | string | "NIL" | "TRUE" | "FALSE" | "(" expression ")" .
unique_ptr<ExpressionNode> Parser::basic_factor() {
    logger_->debug("basic_factor");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        FilePos pos = token->start();
        auto designator = this->designator();
        if (sema_->isConstant(designator->ident())) {
            return sema_->onConstantReference(pos, EMPTY_POS, std::move(designator));
        }
        return sema_->onValueReference(pos, EMPTY_POS, std::move(designator));
    }
    auto tmp = scanner_->next();
    if (token->type() == TokenType::int_literal) {
        auto number = dynamic_cast<const IntLiteralToken *>(tmp.get());
        return sema_->onIntegerLiteral(number->start(), number->end(), number->value());
    } else if (token->type() == TokenType::long_literal) {
        auto number = dynamic_cast<const LongLiteralToken *>(tmp.get());
        return sema_->onIntegerLiteral(number->start(), number->end(), number->value(), true);
    } else if (token->type() == TokenType::float_literal) {
        auto number = dynamic_cast<const FloatLiteralToken *>(tmp.get());
        return sema_->onRealLiteral(number->start(), number->end(), number->value());
    } else if (token->type() == TokenType::double_literal) {
        auto number = dynamic_cast<const DoubleLiteralToken *>(tmp.get());
        return sema_->onRealLiteral(number->start(), number->end(), number->value(), true);
    } else if (token->type() == TokenType::string_literal) {
        auto string = dynamic_cast<const StringLiteralToken *>(tmp.get());
        return sema_->onStringLiteral(string->start(), string->end(), string->value());
    } else if (token->type() == TokenType::boolean_literal) {
        auto boolean = dynamic_cast<const BooleanLiteralToken *>(tmp.get());
        return sema_->onBooleanLiteral(boolean->start(), boolean->end(), boolean->value());
    } else if (token->type() == TokenType::kw_nil) {
        return sema_->onNilLiteral(tmp->start(), tmp->end());
    } else if (token->type() == TokenType::lparen) {
        auto expr = expression();
        token = scanner_->peek();
        if (assertToken(token, TokenType::rparen)) {
            scanner_->next();
        }
        return expr;
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
        logger_->error(ident->start(), "illegal identifier: " + to_string(*ident) + ".");
        return false;
    }
    return true;
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
