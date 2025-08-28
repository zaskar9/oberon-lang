//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_SYMBOLIMPORTER_H
#define OBERON_LANG_SYMBOLIMPORTER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Logger.h"
#include "SymbolFile.h"
#include "SymbolTable.h"
#include "compiler/CompilerConfig.h"
#include "data/ast/ASTContext.h"
#include "data/ast/ModuleNode.h"
#include "data/ast/TypeNode.h"

using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

class SymbolImporter {

private:
    CompilerConfig &config_;
    ASTContext *context_;
    Logger &logger_;
    vector<TypeNode *> types_;
    SymbolTable *symbols_;
    ModuleNode *module_;
    map<unsigned, PointerTypeNode *> forwards_;

    void readDeclaration(SymbolFile *, NodeType);
    TypeNode *readType(SymbolFile *, PointerTypeNode * = nullptr);
    TypeNode *readArrayType(SymbolFile *);
    TypeNode *readPointerType(SymbolFile *);
    TypeNode *readProcedureType(SymbolFile *);
    TypeNode *readRecordType(SymbolFile *);

    ModuleNode *getOrCreateModule(const string &module) const;

public:
    explicit SymbolImporter(CompilerConfig &config, ASTContext *context, SymbolTable *symbols) :
            config_(config), context_(context), logger_(config.logger()), types_(), symbols_(symbols), module_(), forwards_() {};
    ~SymbolImporter() = default;

    ModuleNode *read(const string &module);

};


#endif //OBERON_LANG_SYMBOLIMPORTER_H
