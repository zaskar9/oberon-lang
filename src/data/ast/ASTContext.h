//
// Created by Michael Grossniklaus on 1/28/24.
//

#ifndef OBERON_LANG_ASTCONTEXT_H
#define OBERON_LANG_ASTCONTEXT_H


#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "TypeNode.h"
#include "ArrayTypeNode.h"
#include "ModuleNode.h"
#include "NodeReference.h"
#include "PointerTypeNode.h"
#include "ProcedureTypeNode.h"
#include "RecordTypeNode.h"

using std::filesystem::path;
using std::string;
using std::unique_ptr;
using std::vector;

class ASTContext {

private:
    path file_;
    unique_ptr<ModuleNode> module_;
    vector<unique_ptr<ArrayTypeNode>> array_ts_;
    vector<unique_ptr<RecordTypeNode>> record_ts_;
    vector<unique_ptr<PointerTypeNode>> pointer_ts_;
    vector<unique_ptr<ProcedureTypeNode>> procedure_ts;
    vector<unique_ptr<ModuleNode>> ext_modules_;
    vector<ProcedureNode*> ext_procedures_;

public:
    explicit ASTContext(const path &file) : file_(file), module_(),
            array_ts_(), record_ts_(), pointer_ts_(), procedure_ts(),
            ext_modules_(), ext_procedures_() {};
    ~ASTContext() = default;

    [[nodiscard]] const path &getSourceFileName() const;

    [[nodiscard]] ModuleNode *getTranslationUnit() const;
    void setTranslationUnit(unique_ptr<ModuleNode>);

    ArrayTypeNode *getOrInsertArrayType(Ident *, unsigned int, TypeNode *);
    RecordTypeNode *getOrInsertRecordType(Ident *, vector<unique_ptr<FieldNode>>);
    PointerTypeNode *getOrInsertPointerType(Ident *, TypeNode *);
    ProcedureTypeNode *getOrInsertProcedureType(Ident *, vector<unique_ptr<ParameterNode>>, TypeNode *);

    // mainly for memory management as an anchor for smart pointers
    void addExternalModule(unique_ptr<ModuleNode> module);

    void addExternalProcedure(ProcedureNode *proc);
    [[nodiscard]] ProcedureNode *getExternalProcedure(size_t num) const;
    [[nodiscard]] size_t getExternalProcedureCount() const;

};


#endif //OBERON_LANG_ASTCONTEXT_H
