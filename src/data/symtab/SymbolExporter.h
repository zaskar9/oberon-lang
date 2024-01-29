//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_SYMBOLEXPORTER_H
#define OBERON_LANG_SYMBOLEXPORTER_H


#include "logging/Logger.h"
#include "SymbolTable.h"
#include "SymbolFile.h"
#include "data/ast/ArrayTypeNode.h"
#include "data/ast/ProcedureTypeNode.h"
#include "data/ast/RecordTypeNode.h"
#include <boost/filesystem.hpp>

class SymbolExporter {

private:
    boost::filesystem::path path_;
    Logger *logger_;
    int ref_;

    void writeDeclaration(SymbolFile *file, DeclarationNode *decl);
    void writeType(SymbolFile *file, TypeNode *type);
    void writeArrayType(SymbolFile *file, ArrayTypeNode *type);
    void writeProcedureType(SymbolFile *file, ProcedureTypeNode *type);
    void writeRecordType(SymbolFile *file, RecordTypeNode *type);
    void writeParameter(SymbolFile *file, ParameterNode *param);

public:
    explicit SymbolExporter(boost::filesystem::path &path, Logger *logger) :
            path_(std::move(path)), logger_(logger), ref_() {};
    ~SymbolExporter() = default;

    void write(const std::string &module, SymbolTable *symbols);

};


#endif //OBERON_LANG_SYMBOLEXPORTER_H
