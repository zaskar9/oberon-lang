//
// Created by Michael Grossniklaus on 1/28/24.
//

#ifndef OBERON_LANG_ASTCONTEXT_H
#define OBERON_LANG_ASTCONTEXT_H

#include <memory>
#include <vector>
#include "TypeNode.h"
#include "ArrayTypeNode.h"
#include "ModuleNode.h"
#include "NodeReference.h"
#include "PointerTypeNode.h"
#include "ProcedureTypeNode.h"
#include "RecordTypeNode.h"

using std::unique_ptr;
using std::vector;

class ASTContext {

private:
    unique_ptr<Node> unit_;
    vector<unique_ptr<ArrayTypeNode>> array_ts_;
    vector<unique_ptr<RecordTypeNode>> record_ts_;
    vector<unique_ptr<PointerTypeNode>> pointer_ts_;
    vector<unique_ptr<ProcedureTypeNode>> procedure_ts;
    vector<unique_ptr<TypeReferenceNode>> references_;
    vector<unique_ptr<ModuleNode>> ext_modules_;
    vector<ProcedureNode*> ext_procedures_;

public:
    [[nodiscard]] Node *getTranslationUnit();
    [[deprecated]]
    void setTranslationUnit(unique_ptr<Node>);

//    ModuleNode *createModule(const FilePos &, const FilePos &,
//                             unique_ptr<Ident> ident,
//                             vector<unique_ptr<ImportNode>> imports,
//                             vector<unique_ptr<ConstantDeclarationNode> consts,
//                             vector<unique_ptr<TypeDeclarationNode>> types,
//                             vector<unique_ptr<VariableDeclarationNode>> vars,
//                             vector<unique_ptr<ProcedureNode>> procs,
//                             unique_ptr<StatementSequenceNode> stmts);


    ArrayTypeNode *getOrInsertArrayType(Ident *, unsigned int, TypeNode *);
    RecordTypeNode *getOrInsertRecordType(Ident *, vector<unique_ptr<FieldNode>>);
    PointerTypeNode *getOrInsertPointerType(Ident *, TypeNode *);
    ProcedureTypeNode *getOrInsertProcedureType(Ident *, vector<unique_ptr<ParameterNode>>, TypeNode *);

    [[deprecated]]
    TypeReferenceNode *getOrInsertTypeReference(unique_ptr<QualIdent>);

    // mainly for memory management as an anchor for smart pointers
    void addExternalModule(unique_ptr<ModuleNode> module);

    void addExternalProcedure(ProcedureNode *proc);
    [[nodiscard]] ProcedureNode *getExternalProcedure(size_t num) const;
    [[nodiscard]] size_t getExternalProcedureCount() const;

};


#endif //OBERON_LANG_ASTCONTEXT_H
