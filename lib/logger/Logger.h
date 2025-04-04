/*
 * Logger used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_LOGGER_H
#define OBERON0C_LOGGER_H


#include "global.h"
#include <string>
#include <iostream>

using std::cerr;
using std::cout;
using std::ostream;
using std::string;

enum class LogLevel : unsigned int { DEBUG = 1, INFO = 2, WARNING = 3, ERROR = 4, QUIET = 5 };

class Logger {

private:
    LogLevel level_;
    ostream &out_, &err_;
    int counts_[(unsigned int) LogLevel::QUIET];
    bool werror_;
    string banner_;

    void log(LogLevel level, const string &fileName, int lineNo, int charNo, const string &msg);
    void log(LogLevel level, const string &fileName, const string &msg);
    void log(LogLevel level, const string &msg);

public:
    Logger() : Logger(LogLevel::ERROR, cout, cerr) {};
    Logger(LogLevel level, ostream &out) : Logger(level, out, out) {};
    Logger(LogLevel level, ostream &out, ostream &err) :
        level_(level), out_(out), err_(err), counts_(), werror_(false), banner_() {};
    Logger(const Logger &) = delete;
    Logger& operator=(const LogLevel&) = delete;
    ~Logger() = default;

    void error(const FilePos &, const string &);
    void error(const string &, const string &);
    void warning(const FilePos &, const string &);
    void warning(const string &, const string &);
    void info(const string &);
    void debug(const string &);

    void setBanner(const string &);
    const string &getBanner() const;

    [[nodiscard]] int getDebugCount() const;
    [[nodiscard]] int getInfoCount() const;
    [[nodiscard]] int getWarningCount() const;
    [[nodiscard]] int getErrorCount() const;

    void setLevel(LogLevel level);

    void setWarnAsError(bool werror);

};


#endif //OBERON0C_LOGGER_H
