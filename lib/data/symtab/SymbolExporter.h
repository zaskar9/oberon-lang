//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_SYMBOLEXPORTER_H
#define OBERON_LANG_SYMBOLEXPORTER_H


#include <map>

#include "Logger.h"
#include "SymbolTable.h"
#include "SymbolFile.h"
#include "compiler/CompilerConfig.h"
#include "data/ast/ASTContext.h"
#include "data/ast/ArrayTypeNode.h"
#include "data/ast/ProcedureTypeNode.h"
#include "data/ast/RecordTypeNode.h"

using std::map;

class SymbolExporter {

public:
    SymbolExporter(CompilerConfig &config, ASTContext &context) :
            config_(config), context_(context), logger_(config.logger()), xref_() {}
    ~SymbolExporter() = default;

    void write(const std::string &, SymbolTable *);

private:
    CompilerConfig &config_;
    ASTContext &context_;
    Logger &logger_;
    // cross-references
    unsigned xref_;
    map<TypeNode *, unsigned> xrefs_;
    // forward declarations
    map<TypeNode *, unsigned> fwds_;

    void writeDeclaration(SymbolFile *, DeclarationNode *);
    void writeType(SymbolFile *, TypeNode *);
    void writeArrayType(SymbolFile *, const ArrayTypeNode *);
    void writePointerType(SymbolFile *, const PointerTypeNode *);
    void writeProcedureType(SymbolFile *, ProcedureTypeNode *);
    void writeRecordType(SymbolFile *, const RecordTypeNode *);
    void writeParameter(SymbolFile *, const ParameterNode *);

    bool isExternal(const TypeNode *) const;

};


#endif //OBERON_LANG_SYMBOLEXPORTER_H
