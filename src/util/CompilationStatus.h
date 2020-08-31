/*
 * Status of the current compilation.
 *
 * Created by Michael Grossniklaus on 4/7/20.
 */

#ifndef OBERON_LLVM_COMPILATIONSTATUS_H
#define OBERON_LLVM_COMPILATIONSTATUS_H


#include <vector>
#include "../global.h"
#include "Logger.h"

struct Message {
    FilePos pos;
    LogLevel type;
    std::string msg;
};


class CompilationStatus {

private:
    std::vector<Message> messages_;

public:
    explicit CompilationStatus() : messages_() { };
    ~CompilationStatus() = default;

    void error(const FilePos &pos, const std::string &msg);
    void warning(const FilePos &pos, const std::string &msg);

    void report(Logger logger);

};


#endif //OBERON_LLVM_COMPILATIONSTATUS_H
