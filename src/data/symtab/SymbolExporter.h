//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_SYMBOLEXPORTER_H
#define OBERON_LANG_SYMBOLEXPORTER_H


#include <boost/filesystem.hpp>

#include "SymbolTable.h"
#include "SymbolFile.h"
#include "compiler/CompilerConfig.h"
#include "data/ast/ASTContext.h"
#include "data/ast/ArrayTypeNode.h"
#include "data/ast/ProcedureTypeNode.h"
#include "data/ast/RecordTypeNode.h"
#include "logging/Logger.h"

class SymbolExporter {

private:
    CompilerConfig &config_;
    ASTContext *context_;
    Logger &logger_;
    int ref_;

    void writeDeclaration(SymbolFile *file, DeclarationNode *decl);
    void writeType(SymbolFile *file, TypeNode *type);
    void writeArrayType(SymbolFile *file, ArrayTypeNode *type);
    void writeProcedureType(SymbolFile *file, ProcedureTypeNode *type);
    void writeRecordType(SymbolFile *file, RecordTypeNode *type);
    void writeParameter(SymbolFile *file, ParameterNode *param);

public:
    explicit SymbolExporter(CompilerConfig &config, ASTContext *context) :
            config_(config), context_(context), logger_(config_.logger()), ref_() {};
    ~SymbolExporter() = default;

    void write(const std::string &module, SymbolTable *symbols);

};


#endif //OBERON_LANG_SYMBOLEXPORTER_H
