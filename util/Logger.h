/*
 * Header file of the logger class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */
#ifndef OBERON0C_ERRORLOG_H
#define OBERON0C_ERRORLOG_H

#include <string>
#include <iostream>
#include <sstream>

struct FilePos {
    std::string fileName;
    int lineNo, charNo;
};

enum class LogLevel : unsigned int { DEBUG = 1, INFO = 2, ERROR = 3 };

class Logger
{

private:
    LogLevel level_;
    std::ostream *out_, *err_;

    void log(LogLevel level, const std::string &fileName, int lineNo, int charNo, const std::string &msg) const;
    void log(LogLevel level, const std::string &fileName, const std::string &msg) const;

public:
    explicit Logger();
    explicit Logger(LogLevel level, std::ostream *out, std::ostream *err);
    ~Logger();

    void error(FilePos pos, const std::string &msg) const;
    void error(const std::string &fileName, const std::string &msg) const;
    void info(const std::string &fileName, const std::string &msg) const;
    void debug(const std::string &fileName, const std::string &msg) const;

    void setLevel(LogLevel level);

};

template <typename T>
static std::string to_string(T obj) {
    std::stringstream stream;
    stream << obj;
    return stream.str();
}


#endif //OBERON0C_ERRORLOG_H
