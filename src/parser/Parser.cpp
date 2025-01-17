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
    module(context);
}

// ident = letter { letter | digit } .
unique_ptr<Ident> Parser::ident() {
    auto token = scanner_.peek();
    if (assertToken(token, TokenType::const_ident)) {
        token_ = scanner_.next();
        auto ident = dynamic_cast<const IdentToken*>(token_.get());
        logger_.debug(to_string(*ident));
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
    logger_.debug("qualident");
    auto qualifier = ident();
    if (!sema_.isDefined(qualifier.get()) && scanner_.peek()->type() == TokenType::period) {
        scanner_.next(); // skip the period
        if (assertToken(scanner_.peek(), TokenType::const_ident)) {
            auto identifier = ident();
            return make_unique<QualIdent>(qualifier->start(), identifier->end(), qualifier->name(), identifier->name());
        }
    }
    return make_unique<QualIdent>(qualifier->start(), qualifier->end(), qualifier->name());
}

// identdef = ident [ "*" ] .
unique_ptr<IdentDef> Parser::identdef(bool checkAlphaNum) {
    logger_.debug("identdef");
    auto identifier = ident();
    if (checkAlphaNum) {
        assertOberonIdent(identifier.get());
    }
    auto exp = false;
    if (scanner_.peek()->type() == TokenType::op_times) {
        scanner_.next(); // skip the asterisk
        exp = true;
    }
    return make_unique<IdentDef>(identifier->start(), identifier->end(), identifier->name(), exp);
}

// ident_list = identdef { "," identdef } .
void Parser::ident_list(vector<unique_ptr<IdentDef>> &idents) {
    logger_.debug("ident_list");
    while (true) {
        auto token = scanner_.peek();
        if (token->type() == TokenType::const_ident) {
            idents.push_back(identdef());
        } else {
            logger_.error(token->start(), "identifier expected.");
            break;
        }
        token = scanner_.peek();
        if (token->type() == TokenType::comma) {
            scanner_.next(); // skip comma
        } else if (token->type() == TokenType::const_ident) {
            logger_.error(token->start(), "comma missing.");
        } else if (token->type() == TokenType::colon) {
            break;
        } else {
            logger_.error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
            // [<:>]
            resync({ TokenType::colon });
            break;
        }
    }
}

// module = "MODULE" ident ";" [ import_list ] declarations [ "BEGIN" statement_sequence ] "END" ident "." .
void Parser::module(ASTContext *context) {
    logger_.debug("module");
    token_ = scanner_.next();
    if (!assertToken(token_.get(), TokenType::kw_module)) {
        resync({TokenType::eof});   // [<EOF>]
    }
    auto start = token_->start();
    auto identifier = ident();
    sema_.onTranslationUnitStart(identifier->name());
    context->setTranslationUnit(sema_.onModuleStart(start, std::move(identifier)));
    auto module = context->getTranslationUnit();
    token_ = scanner_.next();
    if (!assertToken(token_.get(), TokenType::semicolon)) {
        // [<IMPORT>, <CONST>, <TYPE>, <VAR>, <PROCEDURE>, <BEGIN>, <END>]
        resync({TokenType::kw_import, TokenType::kw_const, TokenType::kw_type, TokenType::kw_var,
                TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end});
    }
    if (scanner_.peek()->type() == TokenType::kw_import) {
        import_list(module->imports());
    }
    declarations(module->constants(), module->types(), module->variables(), module->procedures());
    token_ = scanner_.next();
    if (token_->type() == TokenType::kw_begin) {
        statement_sequence(module->statements());
        token_ = scanner_.next();
    }
    if (!assertToken(token_.get(), TokenType::kw_end)) {
        resync({TokenType::eof});   // [<EOF>]
    }
    identifier = ident();
    token_ = scanner_.next();
    if (!assertToken(token_.get(), TokenType::period)) {
        resync({TokenType::eof});   // [<EOF>]
    }
    sema_.onModuleEnd(token_->end(), std::move(identifier));
    sema_.onTranslationUnitEnd(module->getIdentifier()->name());
}

// import_list = IMPORT import { "," import } ";" .
void Parser::import_list(vector<unique_ptr<ImportNode>> &imports) {
    logger_.debug("import_list");
    scanner_.next();  // skip IMPORT keyword
    while (true) {
        import(imports);
        auto token = scanner_.peek();
        if (token->type() == TokenType::comma) {
            scanner_.next();  // skip comma
        } else if (token->type() == TokenType::const_ident) {
            logger_.error(token->start(), "comma missing.");
        } else if (token->type() == TokenType::semicolon) {
            scanner_.next();  // skip semicolon
            break;
        } else {
            logger_.error(token->start(), to_string(token->type()) + "unexpected.");
            // [<CONST>, <TYPE>, <VAR>, <PROCEDURE>, <BEGIN>]
            resync({ TokenType::kw_const, TokenType::kw_type, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin });
            break;
        }
    }
}

// import = ident [":=" ident] .
void Parser::import(vector<unique_ptr<ImportNode>> &imports) {
    logger_.debug("import");
    auto token = scanner_.peek();
    if (assertToken(token, TokenType::const_ident)) {
        auto identifier = ident();
        FilePos start = identifier->start();
        FilePos end = identifier->end();
        if (scanner_.peek()->type() == TokenType::op_becomes) {
            scanner_.next(); // skip := operator
            if (assertToken(scanner_.peek(), TokenType::const_ident)) {
                auto name = ident();
                imports.push_back(sema_.onImport(start, end, std::move(identifier), std::move(name)));
            }
        } else {
            imports.push_back(sema_.onImport(start, end, nullptr, std::move(identifier)));
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
    logger_.debug("declarations");
    if (scanner_.peek()->type() == TokenType::kw_const) {
        const_declarations(consts);
    }
    expect({ TokenType::kw_type, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });

    if (scanner_.peek()->type() == TokenType::kw_type) {
        type_declarations(types);
    }
    expect({ TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });

    if (scanner_.peek()->type() == TokenType::kw_var) {
        var_declarations(vars);
    }
    expect({ TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });

    while (scanner_.peek()->type() == TokenType::kw_procedure) {
        procedure_declaration(procs);
    }
    expect({ TokenType::kw_begin, TokenType::kw_end });
}

// TODO const_declaration = identdef "=" const_expression .
// TODO const_expression = expression .
// const_declarations = "CONST" { identdef "=" expression ";" } .
void Parser::const_declarations(vector<unique_ptr<ConstantDeclarationNode>> & consts) {
    logger_.debug("const_declarations");
    FilePos pos = scanner_.next()->start();  // skip CONST keyword and get its position
    while (scanner_.peek()->type() == TokenType::const_ident) {
        FilePos start = scanner_.peek()->start();
        auto ident = identdef();
        auto token = scanner_.next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto decl = sema_.onConstant(start, EMPTY_POS, std::move(ident), expression());
            consts.push_back(std::move(decl));
            token = scanner_.next();
            if (token->type() != TokenType::semicolon) {
                logger_.error(token->start(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
    if (consts.size() == 0) {
        logger_.error(pos, "empty CONST declaration");
    }
}

// type_declarations =  "TYPE" { identdef "=" type ";" } .
void Parser::type_declarations(vector<unique_ptr<TypeDeclarationNode>> & types) {
    logger_.debug("type_declarations");
    FilePos pos = scanner_.next()->start();  // skip TYPE keyword and get its position
    while (scanner_.peek()->type() == TokenType::const_ident) {
        auto pos = scanner_.peek()->start();
        auto ident = identdef();
        auto token = scanner_.next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto node = type();
            auto decl = sema_.onType(pos, EMPTY_POS, std::move(ident), node);
            types.push_back(std::move(decl));
            if (assertToken(scanner_.peek(), TokenType::semicolon)) {
                token = scanner_.next();
            }
        }
    }
    if (types.size() == 0) {
        logger_.error(pos, "empty TYPE declaration");
    }
}

// TODO type = qualident | array_type | record_type | pointer_type | procedure_type.
// type = qualident | array_type | record_type | pointer_type .
TypeNode* Parser::type() {
    logger_.debug("type");
    auto token = scanner_.peek();
    if (token->type() == TokenType::const_ident) {
        FilePos start = token->start();
        FilePos end = token->end();
        return sema_.onTypeReference(start, end, qualident());
    } else if (token->type() == TokenType::kw_array) {
        return array_type();
    } else if (token->type() == TokenType::kw_record) {
        return record_type();
    } else if (token->type() == TokenType::kw_pointer) {
        return pointer_type();
    } else {
        logger_.error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// array_type = "ARRAY" expression { "," expression } "OF" type .
ArrayTypeNode* Parser::array_type() {
    logger_.debug("array_type");
    FilePos pos = scanner_.next()->start();  // skip ARRAY keyword and get its position
    vector<unique_ptr<ExpressionNode>> indices;
    indices.push_back(expression());
    while (scanner_.peek()->type() == TokenType::comma) {
        scanner_.next(); // skip comma
        indices.push_back(expression());
    }
    if (assertToken(scanner_.peek(), TokenType::kw_of)) {
        scanner_.next(); // skip OF keyword
        return sema_.onArrayType(pos, EMPTY_POS, std::move(indices), type());
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// TODO record_type = "RECORD" [ "(" qualident ")" ] [ field_list { ";" field_list } ] END.
// record_type = "RECORD" field_list { ";" field_list } "END" .
RecordTypeNode* Parser::record_type() {
    logger_.debug("record_type");
    FilePos pos = scanner_.next()->start();  // skip RECORD keyword and get its position
    vector<unique_ptr<FieldNode>> fields;
    field_list(fields);
    while (scanner_.peek()->type() == TokenType::semicolon) {
        token_ = scanner_.next();  // skip semicolon but keep token for error reporting in `Parser::field_list`
        field_list(fields);
    }
    if (assertToken(scanner_.peek(), TokenType::kw_end)) {
        scanner_.next();  // skip END keyword
    }
    // [<)>, <;>, <END>]
    // resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return sema_.onRecordType(pos, EMPTY_POS, std::move(fields));
}

// field_list = ident_list ":" type .
void Parser::field_list(vector<unique_ptr<FieldNode>> &fields) {
    logger_.debug("field_list");
    vector<unique_ptr<IdentDef>> idents;
    auto next = scanner_.peek();
    if (next->type() == TokenType::const_ident) {
        ident_list(idents);
        if (!idents.empty()) {
            auto token = scanner_.next();
            if (assertToken(token.get(), TokenType::colon)) {
                auto node = type();
                unsigned index = 0;
                for (auto &&ident: idents) {
                    FilePos start = ident->start();
                    FilePos end = ident->end();
                    fields.push_back(sema_.onField(start, end, std::move(ident), node, index++));
                }
            }
        }
    } else if (next->type() == TokenType::kw_end) {
        logger_.warning(token_->start(), "redundant semicolon.");
    } else {
        logger_.error(next->start(), "identifier expected.");
    }
    // [<;>, <END>]
    resync({ TokenType::semicolon, TokenType::kw_end });
}

// pointer_type = "POINTER" "TO" type .
PointerTypeNode* Parser::pointer_type() {
    logger_.debug("pointer_type");
    FilePos pos = scanner_.next()->start(); // skip POINTER keyword and get its position
    if (assertToken(scanner_.peek(), TokenType::kw_to)) {
        scanner_.next(); // skip TO keyword
        // handle possible forward if next token is a qualident
        if (scanner_.peek()->type() == TokenType::const_ident) {
            return sema_.onPointerType(pos, EMPTY_POS, qualident());
        }
        return sema_.onPointerType(pos, EMPTY_POS, type());
    }
    // [<)>, <;>, <END>]
    resync({ TokenType::semicolon, TokenType::rparen, TokenType::kw_end });
    return nullptr;
}

// var_declarations = "VAR" { ident_list ":" type ";" } .
void Parser::var_declarations(vector<unique_ptr<VariableDeclarationNode>> &vars) {
    logger_.debug("var_declarations");
    FilePos pos = scanner_.next()->start();  // skip VAR keyword and get its position
    while (scanner_.peek()->type() == TokenType::const_ident) {
        vector<unique_ptr<IdentDef>> idents;
        ident_list(idents);
        if (assertToken(scanner_.peek(), TokenType::colon)) {
            scanner_.next();  // skip colon
            auto node = type();
            int index = 0;
            for (auto &&ident : idents) {
                FilePos start = ident->start();
                FilePos end = ident->end();
                auto decl = sema_.onVariable(start, end, std::move(ident), node, index++);
                vars.push_back(std::move(decl));
            }
            if (assertToken(scanner_.peek(), TokenType::semicolon)) {
                scanner_.next();  // skip semicolon
            }
        }
    }
    if (vars.size() == 0) {
        logger_.error(pos, "empty VAR declaration");
    }
}

// procedure_declaration = "PROCEDURE" identdef [ procedure_signature ] ";" ( procedure_body ident | "EXTERN" ) ";" .
void Parser::procedure_declaration(vector<unique_ptr<ProcedureNode>> &procs) {
    logger_.debug("procedure_declaration");
    token_ = scanner_.next(); // skip PROCEDURE keyword
    FilePos start = token_->start();
    auto proc = sema_.onProcedureStart(start, identdef(false));
    auto token = scanner_.peek();
    if (token->type() == TokenType::lparen) {
        // parse formal parameters
        proc->setType(procedure_signature());
    } else if (token->type() == TokenType::colon) {
        // parse return type
        logger_.error(token->start(), "function procedures without parameters must have an empty parameter list.");
        scanner_.next(); // skip ":"
        token = scanner_.peek();
        if (assertToken(token, TokenType::const_ident)) {
            vector<unique_ptr<ParameterNode>> params;
            auto ident = qualident();
            auto type = sema_.onTypeReference(ident->start(), ident->end(), std::move(ident));
            proc->setType(sema_.onProcedureType(token->start(), token->end(), std::move(params), false, type));
        }
    } else {
        // handle missing formal parameters
        vector<unique_ptr<ParameterNode>> params;
        proc->setType(sema_.onProcedureType(token->start(), token->end(), std::move(params), false, nullptr));
    }
    token = scanner_.peek();
    if (assertToken(token, TokenType::semicolon)) {
        scanner_.next();
    }
    unique_ptr<Ident> name;
    if (scanner_.peek()->type() == TokenType::kw_extern) {
        auto ext = scanner_.next(); // skip EXTERN keyword
        proc->setExtern(true);
        if (!config_.hasFlag(Flag::ENABLE_EXTERN)) {
            logger_.error(ext->start(), "external procedure support disabled [-fenable-extern].");
        }
    } else {
        assertOberonIdent(proc->getIdentifier());
        procedure_body(proc);
        name = ident();
    }
    token = scanner_.peek();
    if (assertToken(token, TokenType::semicolon)) {
        token_ = scanner_.next();
    }
    auto node = sema_.onProcedureEnd(token->end(), std::move(name));
    procs.push_back(std::move(node));
    // [<PROCEDURE>, <END>, <BEGIN>]
    //resync({ TokenType::kw_procedure, TokenType::kw_begin, TokenType::kw_end });
}

// procedure_heading = formal_parameters [ ":" type ] .
ProcedureTypeNode* Parser::procedure_signature() {
    logger_.debug("procedure_signature");
    // token_ = scanner_.next(); // skip "(" token
    FilePos start = token_->start();
    vector<unique_ptr<ParameterNode>> params;
    bool varargs = false;
    formal_parameters(params, varargs);
    TypeNode *ret = nullptr;
    if (scanner_.peek()->type() == TokenType::colon) {
        scanner_.next(); // skip colon
        ret = type();
    }
    auto peek = scanner_.peek();
    if (peek->type() != TokenType::semicolon) {
        logger_.error(peek->start(), "unexpected token.");
        // [<;>]
        resync({TokenType::semicolon});
    }
    return sema_.onProcedureType(start, peek->end(), std::move(params), varargs, ret);;
}

// TODO procedure_body = declarations [ "BEGIN" statement_sequence ] [ "RETURN" expression] "END" .
// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
void Parser::procedure_body(ProcedureNode *proc) {
    logger_.debug("procedure_body");
    declarations(proc->constants(), proc->types(), proc->variables(), proc->procedures());
    auto token = scanner_.peek();
    if (token->type() == TokenType::kw_end) {
        scanner_.next(); // skip END keyword
    } else if (assertToken(token, TokenType::kw_begin)) {
        scanner_.next(); // skip BEGIN keyword
        statement_sequence(proc->statements());
        if (assertToken(scanner_.peek(), TokenType::kw_end)) {
            token_ = scanner_.next(); // skip END keyword
        } else {
            resync({TokenType::kw_end});
        }
    }
    // [<ident>]
    resync({ TokenType::const_ident });
}

// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")" .
void Parser::formal_parameters(vector<unique_ptr<ParameterNode>> &params, bool &varargs) {
    logger_.debug("formal_parameters");
    auto token = scanner_.next(); // skip left parenthesis
    if (token->type() == TokenType::lparen) {
        TokenType type = scanner_.peek()->type();
        while (type == TokenType::kw_var || type == TokenType::const_ident || type == TokenType::varargs) {
            fp_section(params, varargs);
            if (scanner_.peek()->type() == TokenType::rparen) {
                token = scanner_.next();
                break;
            }
            if (scanner_.peek()->type() == TokenType::semicolon) {
                token = scanner_.next();  // skip semicolon
                if (varargs) {
                    logger_.error(token->start(), "variadic arguments must be last formal parameter.");
                }
                type = scanner_.peek()->type();
                continue;
            }
            token = scanner_.next();
            logger_.error(token->start(), "; or ) expected, found " + to_string(token->type()) + ".");
        }
        if (type == TokenType::rparen) {
            token = scanner_.next();
        }
        if (token->type() != TokenType::rparen) {
            logger_.error(token->start(), ") expected, found " + to_string(token->type()) + ".");
        }
    }
    // [<:>, <;>]
    resync({ TokenType::colon, TokenType::semicolon });
}

// fp_section = ( [ "VAR" ] ident { "," ident } ":" formal_type | "..." ) .
void Parser::fp_section(vector<unique_ptr<ParameterNode>> &params, bool &varargs) {
    logger_.debug("fp_section");
    if (scanner_.peek()->type() == TokenType::varargs) {
        auto token = scanner_.next(); // skip varargs
        if (config_.hasFlag(Flag::ENABLE_VARARGS)) {
            varargs = true;
        } else {
            logger_.error(token->start(), "variadic arguments support disabled [-fenable-varargs].");
        }
    } else {
        bool var = false;
        if (scanner_.peek()->type() == TokenType::kw_var) {
            scanner_.next(); // skip VAR keyword
            var = true;
        }
        vector<unique_ptr<Ident>> idents;
        while(true) {
            idents.push_back(ident());
            auto token = scanner_. peek();
            if (token->type() == TokenType::comma) {
                scanner_.next(); // skip comma
            } else if (token->type() == TokenType::const_ident) {
                logger_.error(token->start(), "comma missing.");
            } else if (token->type() == TokenType::colon) {
                break;
            } else {
                logger_.error(token->start(), to_string(token->type()) + " unexpected.");
                break;
            }
        }
        auto token = scanner_.next();
        if (token->type() != TokenType::colon) {
            logger_.error(token->start(), ": expected, found " + to_string(token->type()) + ".");
        }
        auto node = formal_type();
        unsigned index = 0;
        for (auto &&ident : idents) {
            FilePos start = ident->start();
            FilePos end = ident->end();
            params.push_back(sema_.onParameter(start, end, std::move(ident), node, var, index++));
        }
    }
    // [<;>, <)>]
    // resync({ TokenType::semicolon, TokenType::rparen });
}

// formal_type = { "ARRAY" "OF" } qualident.
TypeNode *Parser::formal_type() {
    FilePos start;
    unsigned dims = 0;
    while (scanner_.peek()->type() == TokenType::kw_array) {
        token_ = scanner_.next(); // skip keyword "ARRAY"
        start = token_->start();
        if (assertToken(scanner_.peek(), TokenType::kw_of)) {
            scanner_.next(); // skip keyword "OF"
        }
        ++dims;
    }
    auto token = scanner_.peek();
    if (token->type() == TokenType::kw_pointer || token->type() == TokenType::kw_record) {
        logger_.error(token->start(), "formal type cannot be an anonymous record or pointer type.");
    } else if (assertToken(scanner_.peek(), TokenType::const_ident)) {
        auto ident = qualident();
        if (dims == 0) {
            start = ident->start();
        }
        const FilePos end = ident->end();
        return sema_.onTypeReference(start, end, std::move(ident), dims);
    }
    resync({ TokenType::semicolon, TokenType::rparen });
    return nullptr;
}

// statement_sequence = statement { ";" statement } .
void Parser::statement_sequence(StatementSequenceNode *statements) {
    logger_.debug("statement_sequence");
    auto token = scanner_.peek();
    // [<UNTIL>, <ELSIF>, <ELSE>, <END>, <|>]
    if (token->type() == TokenType::kw_end || token->type() == TokenType::kw_elsif ||
        token->type() == TokenType::kw_else || token->type() == TokenType::kw_until ||
        token->type() == TokenType::pipe) {
        logger_.warning(token->start(), "empty statement sequence.");
    } else {
        while (true) {
            statements->addStatement(statement());
            token = scanner_.peek();
            if (token->type() == TokenType::semicolon) {
                token_ = scanner_.next();  // skip semicolon but keep token for error reporting in `Parser::statement`
            } else if (token->type() == TokenType::const_ident ||
                       token->type() == TokenType::kw_if || token->type() == TokenType::kw_case ||
                       token->type() == TokenType::kw_loop || token->type() == TokenType::kw_repeat ||
                       token->type() == TokenType::kw_for || token->type() == TokenType::kw_while ||
                       token->type() == TokenType::kw_exit || token->type() == TokenType::kw_return) {
                logger_.error(token->start(), "semicolon missing.");
            } else if (token->type() == TokenType::kw_end || token->type() == TokenType::kw_elsif ||
                       token->type() == TokenType::kw_else || token->type() == TokenType::kw_until ||
                       token->type() == TokenType::pipe) {
                break;
            } else {
                logger_.error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
                // [<;>] and [<UNTIL>, <ELSIF>, <ELSE>, <END>, <|>]
                resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else,
                    TokenType::kw_until, TokenType::pipe });
            }
        }
    }
}

// statement = ( assignment | procedure_call | if_statement | case_statement
//               while_statement | repeat_statement | for_statement | loop_statement
//               with_statement | "EXIT" | "RETURN" [ expression ] ) .
unique_ptr<StatementNode> Parser::statement() {
    logger_.debug("statement");
    auto token = scanner_.peek();
    switch (token->type()) {
        case TokenType::const_ident: {
            const FilePos pos = token->start();
            vector<unique_ptr<Selector>> selectors;
            auto ident = designator(selectors);
            token = scanner_.peek();
            if (token->type() == TokenType::op_eq) {
                logger_.error(token->start(), "unexpected operator = found, use operator := instead.");
                return nullptr;
            }
            if (token->type() == TokenType::op_becomes) {
                return assignment(sema_.onQualifiedExpression(pos, EMPTY_POS, std::move(ident), std::move(selectors)));
            }
            return sema_.onQualifiedStatement(pos, EMPTY_POS, std::move(ident), std::move(selectors));
        }
        case TokenType::kw_if: return if_statement();
        case TokenType::kw_loop: return loop_statement();
        case TokenType::kw_while: return while_statement();
        case TokenType::kw_repeat: return repeat_statement();
        case TokenType::kw_for: return for_statement();
        case TokenType::kw_case: return case_statement();
        case TokenType::kw_return: {
            token_ = scanner_.next();
            std::set follows{ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until };
            if (follows.contains(scanner_.peek()->type())) {
                return sema_.onReturn(token_->start(), token_->end(), nullptr);
            }
            const FilePos start = token_->start();
            const FilePos end = token->end();
            return sema_.onReturn(start, end, expression());
        }
        case TokenType::semicolon:
            logger_.warning(token_->start(), "redundant semicolon.");
            break;
        case TokenType::kw_end:
        case TokenType::kw_else:
        case TokenType::kw_elsif:
        case TokenType::kw_until:
        case TokenType::pipe:
            logger_.warning(token_->start(), "redundant semicolon.");
            break;
        default: logger_.error(token->start(), "unknown or empty statement.");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return nullptr;
}

// assignment = designator ":=" expression .
unique_ptr<StatementNode> Parser::assignment(unique_ptr<QualifiedExpression> lvalue) {
    logger_.debug("assignment");
    scanner_.next();  // skip assign operator
    FilePos start = lvalue->pos();
    auto statement = sema_.onAssignment(start, EMPTY_POS, std::move(lvalue), expression());
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return statement;
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
unique_ptr<StatementNode> Parser::if_statement() {
    logger_.debug("if_statement");
    token_ = scanner_.next();  // skip IF keyword
    const FilePos ifStart = token_->start();
    unique_ptr<ExpressionNode> ifCond = expression();
    auto thenStmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    vector<unique_ptr<ElseIfNode>> elseIfs;
    auto elseStmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    token_ = scanner_.next();
    if (assertToken(token_.get(), TokenType::kw_then)) {
        statement_sequence(thenStmts.get());
        token_ = scanner_.next();
        while (token_->type() == TokenType::kw_elsif) {
            FilePos elseIfStart = token_->start();
            auto elseIfCond = expression();
            token_ = scanner_.next();
            if (assertToken(token_.get(), TokenType::kw_then)) {
                auto elseIfStmts = make_unique<StatementSequenceNode>(EMPTY_POS);
                statement_sequence(elseIfStmts.get());
                elseIfs.push_back(sema_.onElseIf(elseIfStart, token_->end(),
                    std::move(elseIfCond), std::move(elseIfStmts)));
            }
            token_ = scanner_.next();
        }
        if (token_->type() == TokenType::kw_else) {
            statement_sequence(elseStmts.get());
            token_ = scanner_.next();
        }
        if (token_->type() != TokenType::kw_end) {
            logger_.error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_.onIf(ifStart, token_->end(),
        std::move(ifCond), std::move(thenStmts), std::move(elseIfs), std::move(elseStmts));
}

// loop_statement = "LOOP" statement_sequence "END" .
unique_ptr<StatementNode> Parser::loop_statement() {
    logger_.debug("loop_statement");
    token_ = scanner_.next();  // skip LOOP keyword
    FilePos start = token_->start();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    statement_sequence(stmts.get());
    token_ = scanner_.next();
    if (token_->type() != TokenType::kw_end) {
        logger_.error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_.onLoop(start, token_->end(), std::move(stmts));
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
unique_ptr<StatementNode> Parser::while_statement() {
    logger_.debug("while_statement");
    token_ = scanner_.next();  // skip WHILE keyword
    FilePos start = token_->start();
    auto cond = expression();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    token_ = scanner_.next();
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(stmts.get());
        token_ = scanner_.next();
        if (token_->type() != TokenType::kw_end) {
            logger_.error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_.onWhileLoop(start, token_->end(), std::move(cond), std::move(stmts));
}

// repeat_statement = "REPEAT" statement_sequence "UNTIL" expression .
unique_ptr<StatementNode> Parser::repeat_statement() {
    logger_.debug("repeat_statement");
    token_ = scanner_.next();  // skip REPEAT keyword
    FilePos start = token_->start();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    statement_sequence(stmts.get());
    unique_ptr<ExpressionNode> cond;
    token_ = scanner_.next();
    if (assertToken(token_.get(), TokenType::kw_until)) {
        cond = expression();
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_.onRepeatLoop(start, token_->end(), std::move(cond), std::move(stmts));
}

// for_statement = "FOR" ident ":=" expression "TO" expression [ "BY" const_expression ] "DO" statement_sequence "END" .
unique_ptr<StatementNode> Parser::for_statement() {
    logger_.debug("for_statement");
    token_ = scanner_.next();  // skip FOR keyword
    FilePos start = token_->start();
    auto var = qualident();
    token_ = scanner_.next();
    unique_ptr<ExpressionNode> low;
    if (assertToken(token_.get(), TokenType::op_becomes)) {
        low = expression();
    }
    token_ = scanner_.next();
    unique_ptr<ExpressionNode> high;
    if (assertToken(token_.get(), TokenType::kw_to)) {
        high = expression();
    }
    unique_ptr<ExpressionNode> step;
    if (scanner_.peek()->type() == TokenType::kw_by) {
        scanner_.next();  // skip BY keyword
        step = expression();
    }
    token_ = scanner_.next();
    auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(stmts.get());
    }
    token_ = scanner_.next();
    if (token_->type() != TokenType::kw_end) {
        logger_.error(token_->start(), "END expected, found " + to_string(token_->type()) + ".");
    }
    // [<;>, <END>, <ELSIF>, <ELSE>, <UNTIL>]
    // resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
    return sema_.onForLoop(start, token_->end(),
        std::move(var), std::move(low), std::move(high), std::move(step), std::move(stmts));
}

// case_statement = "CASE" expression "OF" case { "|" case } [ "ELSE" statement_sequence ] "END" .
// case = range_expression_list ":" statement_sequence .
unique_ptr<StatementNode> Parser::case_statement() {
    logger_.debug("case_statement");
    token_ = scanner_.next();  // skip CASE keyword
    const FilePos start = token_->start();
    auto expr = expression();
    vector<unique_ptr<CaseNode>> cases;
    auto elseStmts = make_unique<StatementSequenceNode>(EMPTY_POS);
    if (assertToken(scanner_.peek(), TokenType::kw_of)) {
        scanner_.next();  // skip OF keyword
        do {
            vector<unique_ptr<ExpressionNode>> labels;
            const FilePos caseStart = scanner_.peek()->start();
            range_expression_list(labels);
            if (assertToken(scanner_.peek(), TokenType::colon)) {
                scanner_.next();  // skip colon
            }
            auto stmts = make_unique<StatementSequenceNode>(EMPTY_POS);
            statement_sequence(stmts.get());
            cases.push_back(sema_.onCase(caseStart, EMPTY_POS, std::move(labels), std::move(stmts)));
            token_ = scanner_.next();
        } while (token_->type() == TokenType::pipe);
        if (token_->type() == TokenType::kw_else) {
            statement_sequence(elseStmts.get());
        }
        if (assertToken(scanner_.peek(), TokenType::kw_end)) {
            scanner_.next();  // skip END keyword
        }
    }
    return sema_.onCaseOf(start, token_->end(), std::move(expr), std::move(cases), std::move(elseStmts));
}

// expression_list = expression { "," expression } .
void Parser::expression_list(vector<unique_ptr<ExpressionNode>> &expressions) {
    logger_.debug("expression_list");
    expressions.push_back(expression());
    while (scanner_.peek()->type() == TokenType::comma) {
        token_ = scanner_.next();  // skip comma
        expressions.push_back(expression());
    }
}

// expression = simple_expression [ ( "=" | "#" | "<" | "<=" | ">" | ">=" | "IN" | "IS" ) simple_expression ] .
unique_ptr<ExpressionNode> Parser::expression() {
    logger_.debug("expression");
    auto result = simple_expression();
    TokenType token = scanner_.peek()->type();
    if (token == TokenType::op_eq
        || token == TokenType::op_neq
        || token == TokenType::op_lt
        || token == TokenType::op_leq
        || token == TokenType::op_gt
        || token == TokenType::op_geq
        || token == TokenType::op_in
        || token == TokenType::op_is) {
        token_ = scanner_.next();
        OperatorType op = token_to_operator(token_->type());
        auto rhs = simple_expression();
        result = sema_.onBinaryExpression(result->pos(), EMPTY_POS, op, std::move(result), std::move(rhs));
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
    logger_.debug("simple_expression");
    unique_ptr<ExpressionNode> expr;
    TokenType token = scanner_.peek()->type();
    if (token == TokenType::op_plus) {
        token_ = scanner_.next();
        expr = term();
    } else if (token == TokenType::op_minus) {
        token_ = scanner_.next();
        expr = sema_.onUnaryExpression(token_->start(), EMPTY_POS, OperatorType::NEG, term());
    } else {
        expr = term();
    }
    token = scanner_.peek()->type();
    while (token == TokenType::op_plus
           || token == TokenType::op_minus
           || token == TokenType::op_or) {
        token_ = scanner_.next();
        OperatorType op = token_to_operator(token_->type());
        expr = sema_.onBinaryExpression(token_->start(), EMPTY_POS, op, std::move(expr), term());
        token = scanner_.peek()->type();
    }
    return expr;
}

// term = factor { ( "*" | "/" | "DIV" | "MOD" | "&" ) factor } .
unique_ptr<ExpressionNode> Parser::term() {
    logger_.debug("term");
    auto expr = factor();
    TokenType token = scanner_.peek()->type();
    while (token == TokenType::op_times
           || token == TokenType::op_divide
           || token == TokenType::op_div
           || token == TokenType::op_mod
           || token == TokenType::op_and) {
        token_ = scanner_.next();
        OperatorType op = token_to_operator(token_->type());
        expr = sema_.onBinaryExpression(token_->start(), EMPTY_POS, op, std::move(expr), factor());
        token = scanner_.peek()->type();
    }
    return expr;
}

// factor = [ "+" | "-" | "~" ] basic_factor .
unique_ptr<ExpressionNode> Parser::factor() {
    logger_.debug("factor");
    auto token = scanner_.peek();
    if (token->type() == TokenType::op_plus) {
        scanner_.next();   // skip plus sign
    } else if (token->type() == TokenType::op_minus) {
        auto op = scanner_.next();
        return sema_.onUnaryExpression(op->start(), EMPTY_POS, OperatorType::NEG, basic_factor());
    } else if (token->type() == TokenType::op_not) {
        auto op = scanner_.next();
        return sema_.onUnaryExpression(op->start(), EMPTY_POS, OperatorType::NOT, basic_factor());
    }
    return basic_factor();
}

// basic_factor = designator | set |  "(" expression ")" | number | string | TRUE | FALSE | NIL | "~" factor.
unique_ptr<ExpressionNode> Parser::basic_factor() {
    logger_.debug("basic_factor");
    auto token = scanner_.peek();
    if (token->type() == TokenType::const_ident) {
        FilePos pos = token->start();
        vector<unique_ptr<Selector>> selectors;
        auto ident = designator(selectors);
        if (sema_.isConstant(ident.get())) {
            return sema_.onQualifiedConstant(pos, EMPTY_POS, std::move(ident), std::move(selectors));
        }
        return sema_.onQualifiedExpression(pos, EMPTY_POS, std::move(ident), std::move(selectors));
    } else if (token->type() == TokenType::lbrace) {
        return set();
    } else if (token->type() == TokenType::lparen) {
        scanner_.next();  // skip opening parenthesis
        auto expr = expression();
        token = scanner_.peek();
        if (assertToken(token, TokenType::rparen)) {
            scanner_.next();  // skip closing parenthesis
        }
        return expr;
    }
    auto tmp = scanner_.next();
    if (token->type() == TokenType::short_literal) {
        auto number = dynamic_cast<const ShortLiteralToken *>(tmp.get());
        return sema_.onIntegerLiteral(number->start(), number->end(), number->value(), TypeKind::SHORTINT);
    } else if (token->type() == TokenType::int_literal) {
        auto number = dynamic_cast<const IntLiteralToken *>(tmp.get());
        return sema_.onIntegerLiteral(number->start(), number->end(), number->value(), TypeKind::INTEGER);
    } else if (token->type() == TokenType::long_literal) {
        auto number = dynamic_cast<const LongLiteralToken *>(tmp.get());
        return sema_.onIntegerLiteral(number->start(), number->end(), number->value(), TypeKind::LONGINT);
    } else if (token->type() == TokenType::float_literal) {
        auto number = dynamic_cast<const FloatLiteralToken *>(tmp.get());
        return sema_.onRealLiteral(number->start(), number->end(), number->value(), TypeKind::REAL);
    } else if (token->type() == TokenType::double_literal) {
        auto number = dynamic_cast<const DoubleLiteralToken *>(tmp.get());
        return sema_.onRealLiteral(number->start(), number->end(), number->value(), TypeKind::LONGREAL);
    } else if (token->type() == TokenType::string_literal) {
        auto string = dynamic_cast<const StringLiteralToken *>(tmp.get());
        return sema_.onStringLiteral(string->start(), string->end(), string->value());
    } else if (token->type() == TokenType::char_literal) {
        auto character = dynamic_cast<const CharLiteralToken *>(tmp.get());
        return sema_.onCharLiteral(character->start(), character->end(), character->value());
    } else if (token->type() == TokenType::boolean_literal) {
        auto boolean = dynamic_cast<const BooleanLiteralToken *>(tmp.get());
        return sema_.onBooleanLiteral(boolean->start(), boolean->end(), boolean->value());
    } else if (token->type() == TokenType::kw_nil) {
        return sema_.onNilLiteral(tmp->start(), tmp->end());
    } else {
        logger_.error(token->start(), "unexpected token: " + to_string(token->type()) + ".");
        return nullptr;
    }
}

// designator = qualident { selector } .
unique_ptr<QualIdent> Parser::designator(vector<unique_ptr<Selector>> &selectors) {
    logger_.debug("designator");
    auto ident = qualident();
    auto token = scanner_.peek();
    while (token->type() == TokenType::period ||
           token->type() == TokenType::lbrack ||
           token->type() == TokenType::caret ||
           token->type() == TokenType::lparen) {
        auto sel = selector();
        if (sel) {
            selectors.push_back(std::move(sel));
        } else {
            break;
        }
        token = scanner_.peek();
    }
    return ident;
}

// selector = "." ident | "[" expression_list "]" | "^" | "(" qualident | [ expression_list ] ")" .
unique_ptr<Selector> Parser::selector() {
    logger_.debug("selector");
    auto token = scanner_.peek();
    if (token->type() == TokenType::period) {
        token_ = scanner_.next();
        auto pos = token_->start();
        return make_unique<RecordField>(pos, ident());
    } else if (token->type() == TokenType::lbrack) {
        auto pos = token->start();
        token_ = scanner_.next();
        vector<unique_ptr<ExpressionNode>> expressions;
        expression_list(expressions);
        if (expressions.empty()) {
            logger_.error(token_->start(), "expression expected.");
        }
        token_ = scanner_.next();
        if (token_->type() != TokenType::rbrack) {
            logger_.error(token_->start(), "] expected, found " + to_string(token_->type()) + ".");
        }
        return make_unique<ArrayIndex>(pos, std::move(expressions));
    } else if (token->type() == TokenType::caret) {
        token_ = scanner_.next();
        return make_unique<Dereference>(token_->start());
    } else if (token->type() == TokenType::lparen) {
        token_ = scanner_.next();  // skip left parenthesis
        FilePos start = token_->start();
        vector<std::unique_ptr<ExpressionNode>> params;
        auto debug = scanner_.peek()->type();
        if (debug != TokenType::rparen) {
            expression_list(params);
        }
        logger_.debug("actual_parameters");
        token_ = scanner_.next();  // skip right parenthesis
        assertToken(token_.get(), TokenType::rparen);
        return make_unique<ActualParameters>(start, std::move(params));;
    }
    logger_.error(token_->start(), "selector expected.");
    return nullptr;
}

// set = "{" [ range_expression { "," range_expression } ] "}" .
unique_ptr<ExpressionNode> Parser::set() {
    logger_.debug("set");
    vector<unique_ptr<ExpressionNode>> elements;
    auto token = scanner_.next();   // skip opening brace
    if (scanner_.peek()->type() == TokenType::rbrace) {
        token_ = scanner_.next();
        return sema_.onSetExpression(token->start(), token_->end(), std::move(elements));
    }
    range_expression_list(elements);
    if (assertToken(scanner_.peek(), TokenType::rbrace)) {
        token_ = scanner_.next();   // skip closing brace
    }
    return sema_.onSetExpression(token->start(), token_->end(), std::move(elements));
}

// range_expression = expression [ ".." expression ] .
unique_ptr<ExpressionNode> Parser::range_expression() {
    logger_.debug("range_expression");
    auto expr = expression();
    if (scanner_.peek()->type() == TokenType::range) {
        scanner_.next();   // skip range indicator
        const FilePos start = expr->pos();
        return sema_.onRangeExpression(start, EMPTY_POS, std::move(expr), expression());
    }
    return expr;
}

// range_expression_list = range_expression { "," range_expression }.
void Parser::range_expression_list(vector<unique_ptr<ExpressionNode>>& expressions) {
    logger_.debug("range_expression_list");
    expressions.push_back(range_expression());
    while (scanner_.peek()->type() == TokenType::comma) {
        token_ = scanner_.next();  // skip comma
        expressions.push_back(range_expression());
    }
}

bool Parser::assertToken(const Token *token, TokenType expected) {
    if (token->type() == expected) {
        return true;
    }
    logger_.error(token->start(), to_string(expected) + " expected, found " + to_string(token->type()) + ".");
    return false;
}

bool Parser::assertOberonIdent(const Ident *ident) {
    if (ident->name().find('_') != std::string::npos) {
        logger_.error(ident->start(), "illegal identifier: " + to_string(*ident) + ".");
        return false;
    }
    return true;
}

void Parser::resync(std::set<TokenType> types) {
    auto type = scanner_.peek()->type();
    while (type != TokenType::eof && types.find(type) == types.end()) {
        scanner_.next();
        type = scanner_.peek()->type();
    }
}

// Check expected next token
void Parser::expect(std::set<TokenType> exp) {
    auto type = scanner_.peek()->type();
    if (exp.find(type) == exp.end()) {
        auto token = scanner_.peek();
        logger_.error(token->start(), "unexpected token "+ to_string(token->type()) + ".");
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
        case TokenType::op_in:     return OperatorType::IN;
        case TokenType::op_is:     return OperatorType::IS;
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
            std::cerr << "Parser cannot map token type to operator." << std::endl;
            exit(1);
    }
}
