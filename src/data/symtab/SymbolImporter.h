//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_SYMBOLIMPORTER_H
#define OBERON_LANG_SYMBOLIMPORTER_H

#include <memory>
#include <string>
#include <vector>

#include "SymbolFile.h"
#include "SymbolTable.h"
#include "compiler/CompilerConfig.h"
#include "data/ast/ASTContext.h"
#include "data/ast/ModuleNode.h"
#include "data/ast/TypeNode.h"
#include "logging/Logger.h"

using std::string;
using std::vector;
using std::unique_ptr;

class SymbolImporter {

private:
    CompilerConfig &config_;
    ASTContext *context_;
    Logger &logger_;
    vector<TypeNode*> types_;
    SymbolTable *symbols_;
    // std::unique_ptr<ModuleNode> module_;

    void readDeclaration(SymbolFile *, NodeType, ModuleNode *);
    TypeNode *readType(SymbolFile *);
    TypeNode *readArrayType(SymbolFile *);
    TypeNode *readProcedureType(SymbolFile *);
    TypeNode *readRecordType(SymbolFile *);
    TypeNode *readParameter(SymbolFile *);

public:
    explicit SymbolImporter(CompilerConfig &config, ASTContext *context) :
            config_(config), context_(context), logger_(config.logger()), types_(), symbols_() {};
    ~SymbolImporter() = default;

    unique_ptr<ModuleNode> read(const string &module, SymbolTable *symbols);
    unique_ptr<ModuleNode> read(const string &alias, const string &name, SymbolTable *symbols);

};


#endif //OBERON_LANG_SYMBOLIMPORTER_H
