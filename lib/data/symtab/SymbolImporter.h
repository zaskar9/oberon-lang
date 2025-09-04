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
#include "system/OberonSystem.h"

using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

class SymbolImporter {

public:
    explicit SymbolImporter(CompilerConfig &config, ASTContext &context, OberonSystem &system) :
            config_(config), context_(context), logger_(config.logger()), system_(system),
            symbols_(system.getSymbolTable()), module_() {}
    ~SymbolImporter() = default;

    ModuleNode *read(const string &);

private:
    CompilerConfig &config_;
    ASTContext &context_;
    Logger &logger_;
    OberonSystem &system_;
    SymbolTable *symbols_;
    ModuleNode *module_;
    vector<TypeNode *> types_;
    // cross-references
    map<unsigned, TypeNode*> xrefs_;
    // forward declarations
    map<unsigned, PointerTypeNode *> fwds_;

    void readDeclaration(SymbolFile *, NodeType);
    TypeNode *readType(SymbolFile *, const TypeDeclarationNode * = nullptr, PointerTypeNode * = nullptr);
    TypeNode *readArrayType(SymbolFile *);
    TypeNode *readPointerType(SymbolFile *);
    TypeNode *readProcedureType(SymbolFile *);
    TypeNode *readRecordType(SymbolFile *);

    void setXRef(unsigned ref, TypeNode *type);
    [[nodiscard]] TypeNode *getXRef(unsigned ref) const;

    [[nodiscard]] ModuleNode *getOrCreateModule(const string &) const;

};


#endif //OBERON_LANG_SYMBOLIMPORTER_H
