//
// Created by Michael Grossniklaus on 1/28/24.
//

#ifndef OBERON_LANG_ASTCONTEXT_H
#define OBERON_LANG_ASTCONTEXT_H


#include <filesystem>
#include <map>
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
using std::map;
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
    map<string, unique_ptr<ModuleNode>> ext_modules_;
    vector<ProcedureNode*> ext_procedures_;

public:
    explicit ASTContext(const path &file) : file_(file), module_(),
            array_ts_(), record_ts_(), pointer_ts_(), procedure_ts(),
            ext_modules_(), ext_procedures_() {};
    ~ASTContext() = default;

    [[nodiscard]] const path &getSourceFileName() const;

    [[nodiscard]] ModuleNode *getTranslationUnit() const;
    void setTranslationUnit(unique_ptr<ModuleNode>);

    [[deprecated]]
    ArrayTypeNode *getOrInsertArrayType(const FilePos &, const FilePos &, unsigned, TypeNode *);
    ArrayTypeNode *getOrInsertArrayType(const FilePos &, const FilePos &, unsigned, vector<unsigned>, vector<TypeNode *>);
    RecordTypeNode *getOrInsertRecordType(const FilePos &, const FilePos &, RecordTypeNode *, vector<unique_ptr<FieldNode>>);
    PointerTypeNode *getOrInsertPointerType(const FilePos &, const FilePos &, TypeNode *);
    ProcedureTypeNode *getOrInsertProcedureType(const FilePos &, const FilePos &,
                                                vector<unique_ptr<ParameterNode>>, bool, TypeNode *);

    // mainly for memory management as an anchor for smart pointers
    void addExternalModule(unique_ptr<ModuleNode> module);
    ModuleNode* getExternalModule(const string &name);

    void addExternalProcedure(ProcedureNode *proc);
    [[nodiscard]] ProcedureNode *getExternalProcedure(size_t num) const;
    [[nodiscard]] size_t getExternalProcedureCount() const;

};


#endif //OBERON_LANG_ASTCONTEXT_H
