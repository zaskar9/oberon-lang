/*
 * Scanner used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON_LANG_SCANNER_H
#define OBERON_LANG_SCANNER_H


#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "global.h"
#include "Logger.h"
#include "Token.h"

/**
 * @brief Tag exception thrown by the scanner on unrecoverable errors.
 *
 * Thrown after the error has already been recorded via the @c Logger, so
 * callers only need to catch this type to terminate cleanly — no additional
 * message is required.
 */
struct ScannerError : std::exception {
    ~ScannerError() override;
};

/**
 * @brief Handwritten scanner (lexer) for the Oberon programming language.
 *
 * Reads a source file character by character and produces a stream of typed
 * @c Token objects.  Identifiers are matched against a keyword table so that
 * reserved words are returned as their own token types.  Numeric, character,
 * and string literals are fully evaluated during scanning.
 *
 * The scanner supports arbitrary lookahead through an internal token buffer:
 * @c peek() and @c peekAhead() inspect tokens without consuming them, while
 * @c next() removes and returns the next token from the stream.
 *
 * On unrecoverable errors (file not found, I/O failure, unterminated literal
 * or comment) the scanner logs a diagnostic and throws @c ScannerError.
 */
class Scanner {

public:
    /**
     * @brief Opens @p path and primes the scanner by reading the first character.
     * @param logger  Diagnostic sink used for all error and warning messages.
     * @param path    Path to the Oberon source file to scan.
     * @throws ScannerError if the file cannot be opened.
     */
    Scanner(Logger &logger, std::filesystem::path path);

    /** @brief Closes the source file. */
    ~Scanner();

    /**
     * @brief Returns the next token without consuming it.
     *
     * Scans one token if the lookahead buffer is empty.  Repeated calls
     * return the same token until @c next() is called.
     *
     * @return Non-owning pointer to the next token; valid until @c next() is called.
     */
    const Token* peek();

    /**
     * @brief Extends the lookahead buffer by one token and returns the previous end.
     *
     * Useful when the parser needs to inspect two tokens ahead without consuming
     * either.  After this call, @c peek() still returns the same front-of-buffer
     * token, but the buffer now also contains the token that follows it.
     *
     * @return Non-owning pointer to the token that was at the back of the buffer
     *         before the new token was scanned; valid until @c next() is called.
     */
    const Token* peekAhead();

    /**
     * @brief Removes and returns the next token from the stream.
     * @return Owning pointer to the consumed token.
     */
    std::unique_ptr<const Token> next();

    /**
     * @brief Escapes special characters in @p str using C-style backslash sequences.
     *
     * For example, a newline byte becomes the two-character sequence @c \\n.
     * This is the inverse of @c unescape().
     *
     * @param str  String whose special characters are to be escaped.
     * @return A new string with all special characters replaced by escape sequences.
     */
    static std::string escape(const string &str);

    /**
     * @brief Replaces C-style backslash escape sequences in @p str with their
     *        corresponding characters.
     *
     * Processes the string left-to-right so that sequences such as @c \\\\n
     * are handled correctly: @c \\\\ becomes a single backslash, and the
     * following @c n remains a literal character.  This is the inverse of
     * @c escape().
     *
     * @param str  String containing backslash escape sequences.
     * @return A new string with all recognized escape sequences substituted.
     */
    static std::string unescape(const string &str);

private:
    Logger &logger_;
    std::filesystem::path path_;
    std::queue<std::unique_ptr<const Token>> tokens_;
    int lineNo_, charNo_;
    char ch_;
    bool eof_;
    std::unordered_map<std::string, TokenType> keywords_;
    std::ifstream file_;

    void init();
    void read();
    void seek(const FilePos &);
    FilePos current();
    std::unique_ptr<const Token> scanToken();
    std::unique_ptr<const Token> scanIdent();
    std::unique_ptr<const Token> scanNumber();
    // std::unique_ptr<const Token> scanCharacter();
    std::unique_ptr<const Token> scanString();
    void scanComment(const FilePos &);

};


#endif //OBERON_LANG_SCANNER_H
