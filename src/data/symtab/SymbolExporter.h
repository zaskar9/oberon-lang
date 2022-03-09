//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_SYMBOLEXPORTER_H
#define OBERON_LANG_SYMBOLEXPORTER_H


#include <fstream>
#include <boost/filesystem.hpp>
#include "SymbolTable.h"
#include "../../logging/Logger.h"

class SymbolExporter {

private:
    [[maybe_unused]] Logger *logger_;
    std::ofstream file_;

public:
    explicit SymbolExporter(Logger *logger) : logger_(logger), file_() { };

    void write(const std::string &module, SymbolTable *symbols, boost::filesystem::path path);

};


#endif //OBERON_LANG_SYMBOLEXPORTER_H
