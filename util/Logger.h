/*
 * Header file of the logger class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */
#ifndef OBERON0C_ERRORLOG_H
#define OBERON0C_ERRORLOG_H

#include <string>

struct FilePos {
    std::string fileName;
    int lineNo, charNo;
};

class Logger
{

public:
    explicit Logger();
    ~Logger();

    void error(const FilePos pos, const std::string &msg) const;
    void error(const std::string &fileName, const std::string &msg) const;
    void info(const std::string &fileName, const std::string &msg) const;

};


#endif //OBERON0C_ERRORLOG_H
