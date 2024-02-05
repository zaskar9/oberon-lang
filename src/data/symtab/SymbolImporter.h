//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_SYMBOLIMPORTER_H
#define OBERON_LANG_SYMBOLIMPORTER_H

#include <memory>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "logging/Logger.h"
#include "compiler/CompilerFlags.h"
#include "data/ast/ModuleNode.h"
#include "data/ast/TypeNode.h"
#include "SymbolTable.h"
#include "SymbolFile.h"
#include "data/ast/ASTContext.h"

using boost::filesystem::path;
using std::string;
using std::vector;
using std::unique_ptr;

class SymbolImporter {

private:
    CompilerFlags *flags_;
    ASTContext *context_;
    path path_;
    Logger *logger_;
    vector<TypeNode*> types_;
    SymbolTable *symbols_;
    // std::unique_ptr<ModuleNode> module_;

    void readDeclaration(SymbolFile *, NodeType,
                         const string &, const string &,
                         vector<unique_ptr<ConstantDeclarationNode>> &,
                         vector<unique_ptr<TypeDeclarationNode>> &,
                         vector<unique_ptr<VariableDeclarationNode>> &,
                         vector<unique_ptr<ProcedureNode>> &);
    TypeNode *readType(SymbolFile *);
    TypeNode *readArrayType(SymbolFile *);
    TypeNode *readProcedureType(SymbolFile *);
    TypeNode *readRecordType(SymbolFile *);
    TypeNode *readParameter(SymbolFile *);

public:
    explicit SymbolImporter(CompilerFlags *flags, ASTContext *context, path &path, Logger *logger) :
            flags_(flags), context_(context), path_(std::move(path)), logger_(logger), types_(), symbols_() {};
    ~SymbolImporter() = default;

    unique_ptr<ModuleNode> read(const string &module, SymbolTable *symbols);
    unique_ptr<ModuleNode> read(const string &alias, const string &name, SymbolTable *symbols);

};


#endif //OBERON_LANG_SYMBOLIMPORTER_H
