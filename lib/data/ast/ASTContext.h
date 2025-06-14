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

public:
    explicit ASTContext(const path &file) : file_(file) {}
    ~ASTContext() = default;

    [[nodiscard]] const path &getSourceFileName() const;

    [[nodiscard]] ModuleNode *getTranslationUnit() const;
    void setTranslationUnit(unique_ptr<ModuleNode>);

    ArrayTypeNode *getOrInsertArrayType(const FilePos &, const FilePos &,
                                        unsigned, vector<unsigned>, vector<TypeNode *>, ModuleNode * = nullptr);
    RecordTypeNode *getOrInsertRecordType(const FilePos &, const FilePos &,
                                          RecordTypeNode *, vector<unique_ptr<FieldNode>>, ModuleNode * = nullptr);
    PointerTypeNode *getOrInsertPointerType(const FilePos &, const FilePos &, TypeNode *, ModuleNode * = nullptr);
    ProcedureTypeNode *getOrInsertProcedureType(const FilePos &, const FilePos &,
                                                vector<unique_ptr<ParameterNode>>, bool, TypeNode *, ModuleNode * = nullptr);

    // mainly for memory management as an anchor for smart pointers
    void addExternalModule(unique_ptr<ModuleNode> module);
    ModuleNode* getExternalModule(const string &name);

    void addExternalProcedure(ProcedureDeclarationNode *proc);
    [[nodiscard]] ProcedureDeclarationNode *getExternalProcedure(size_t num) const;
    [[nodiscard]] size_t getExternalProcedureCount() const;

private:
    path file_;
    unique_ptr<ModuleNode> module_;
    vector<unique_ptr<ArrayTypeNode>> array_ts_;
    vector<unique_ptr<RecordTypeNode>> record_ts_;
    vector<unique_ptr<PointerTypeNode>> pointer_ts_;
    vector<unique_ptr<ProcedureTypeNode>> procedure_ts;
    map<string, unique_ptr<ModuleNode>> ext_modules_;
    vector<ProcedureDeclarationNode*> ext_procedures_;

};


#endif //OBERON_LANG_ASTCONTEXT_H
