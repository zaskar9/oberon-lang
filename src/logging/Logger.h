/*
 * Logger used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_LOGGER_H
#define OBERON0C_LOGGER_H


#include <string>
#include <iostream>
#include "global.h"

enum class LogLevel : unsigned int { DEBUG = 1, INFO = 2, WARNING = 3, ERROR = 4, QUIET = 5 };

class Logger {

private:
    LogLevel level_;
    std::ostream *out_, *err_;
    int counts_[(unsigned int) LogLevel::QUIET];

    void log(LogLevel level, const std::string &fileName, int lineNo, int charNo, const std::string &msg);
    void log(LogLevel level, const std::string &fileName, const std::string &msg);

public:

    explicit Logger() : Logger(LogLevel::ERROR, &std::cout, &std::cerr) { };
    explicit Logger(LogLevel level, std::ostream *out) : Logger(level, out, out) { };
    explicit Logger(LogLevel level, std::ostream *out, std::ostream *err) :
            level_(level), out_(out), err_(err), counts_() { };
    ~Logger() = default;

    void error(const FilePos &pos, const std::string &msg);
    void error(const std::string &fileName, const std::string &msg);
    void warning(const FilePos &pos, const std::string &msg);
    void warning(const std::string &fileName, const std::string &msg);
    void info(const std::string &fileName, const std::string &msg);
    void debug(const std::string &fileName, const std::string &msg);

    [[nodiscard]] int getDebugCount() const;
    [[nodiscard]] int getInfoCount() const;
    [[nodiscard]] int getWarningCount() const;
    [[nodiscard]] int getErrorCount() const;

    void setLevel(LogLevel level);

};


#endif //OBERON0C_LOGGER_H
