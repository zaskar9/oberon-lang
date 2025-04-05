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

private:
    CompilerConfig &config_;
    ASTContext *context_;
    Logger &logger_;
    int ref_;
    map<TypeNode *, int> refs_;

    void writeDeclaration(SymbolFile *, DeclarationNode *);
    void writeType(SymbolFile *, TypeNode *);
    void writeArrayType(SymbolFile *, ArrayTypeNode *);
    void writePointerType(SymbolFile *, PointerTypeNode *);
    void writeProcedureType(SymbolFile *, ProcedureTypeNode *);
    void writeRecordType(SymbolFile *, RecordTypeNode *);
    void writeParameter(SymbolFile *, ParameterNode *);

public:
    explicit SymbolExporter(CompilerConfig &config, ASTContext *context) :
            config_(config), context_(context), logger_(config_.logger()), ref_(), refs_() {};
    ~SymbolExporter() = default;

    void write(const std::string &module, SymbolTable *symbols);

};


#endif //OBERON_LANG_SYMBOLEXPORTER_H
