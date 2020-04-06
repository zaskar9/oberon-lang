/*
 * Logger used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */
#ifndef OBERON0C_LOGGER_H
#define OBERON0C_LOGGER_H

#include <string>
#include <iostream>
#include <sstream>

struct FilePos {
    std::string fileName;
    int lineNo, charNo;
};

enum class LogLevel : unsigned int { DEBUG = 1, INFO = 2, WARNING = 3, ERROR = 4 };

class Logger {

private:
    LogLevel level_;
    std::ostream *out_, *err_;
    int debugs_, infos_, warnings_, errors_;

    void log(LogLevel level, const std::string &fileName, int lineNo, int charNo, const std::string &msg);
    void log(LogLevel level, const std::string &fileName, const std::string &msg);

public:

    explicit Logger() : Logger(LogLevel::ERROR, &std::cout, &std::cerr) { };
    explicit Logger(LogLevel level, std::ostream *out) : Logger(level, out, out) { };
    explicit Logger(LogLevel level, std::ostream *out, std::ostream *err) :
            level_(level), out_(out), err_(err), debugs_(0), infos_(0), warnings_(0), errors_(0) { };
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

template <typename T>
static std::string to_string(T obj) {
    std::stringstream stream;
    stream << obj;
    return stream.str();
}


#endif //OBERON0C_LOGGER_H
