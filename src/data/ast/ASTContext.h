//
// Created by Michael Grossniklaus on 1/28/24.
//

#ifndef OBERON_LANG_ASTCONTEXT_H
#define OBERON_LANG_ASTCONTEXT_H

#include <memory>
#include <vector>
#include "TypeNode.h"
#include "ArrayTypeNode.h"
#include "RecordTypeNode.h"
#include "PointerTypeNode.h"
#include "ProcedureTypeNode.h"
#include "NodeReference.h"

using std::unique_ptr;
using std::vector;

class ASTContext {

private:
    vector<unique_ptr<ArrayTypeNode>> array_ts_;
    vector<unique_ptr<RecordTypeNode>> record_ts_;
    vector<unique_ptr<PointerTypeNode>> pointer_ts_;
    vector<unique_ptr<ProcedureTypeNode>> procedure_ts;

public:
    ArrayTypeNode *getOrInsertArrayType(unsigned int, TypeNode *);
    RecordTypeNode *getOrInsertRecordType(vector<unique_ptr<FieldNode>>);
    PointerTypeNode *getOrInsertPointerType(TypeNode *);
    ProcedureTypeNode *getOrInsertProcedureNode(vector<unique_ptr<ParameterNode>>, TypeNode *);

    TypeReferenceNode *getOrInsertTypeReference(unique_ptr<QualIdent>);

};


#endif //OBERON_LANG_ASTCONTEXT_H
