//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_SYMBOLIMPORTER_H
#define OBERON_LANG_SYMBOLIMPORTER_H


#include "logging/Logger.h"
#include "compiler/CompilerFlags.h"
#include "data/ast/ModuleNode.h"
#include "data/ast/TypeNode.h"
#include "SymbolTable.h"
#include "SymbolFile.h"
#include "data/ast/ASTContext.h"
#include <boost/filesystem.hpp>

class SymbolImporter {

private:
    CompilerFlags *flags_;
    ASTContext *context_;
    boost::filesystem::path path_;
    Logger *logger_;
    std::vector<TypeNode*> types_;
    SymbolTable *symbols_;
    std::unique_ptr<ModuleNode> module_;

    void readDeclaration(SymbolFile *file, NodeType nodeType);
    TypeNode *readType(SymbolFile *file);
    TypeNode *readArrayType(SymbolFile *file);
    TypeNode *readProcedureType(SymbolFile *file);
    TypeNode *readRecordType(SymbolFile *file);
    TypeNode *readParameter(SymbolFile *file);

public:
    explicit SymbolImporter(CompilerFlags *flags, ASTContext *context, boost::filesystem::path &path, Logger *logger) :
            flags_(flags), context_(context), path_(std::move(path)), logger_(logger), types_(), symbols_(), module_() {};
    ~SymbolImporter() = default;

    std::unique_ptr<ModuleNode> read(const std::string &module, SymbolTable *symbols);
    std::unique_ptr<ModuleNode> read(const std::string &alias, const std::string &module, SymbolTable *symbols);

};


#endif //OBERON_LANG_SYMBOLIMPORTER_H
