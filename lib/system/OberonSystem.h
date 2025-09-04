//
// Created by Michael Grossniklaus on 10/23/22.
//

#ifndef OBERON_LANG_OBERONSYSTEM_H
#define OBERON_LANG_OBERONSYSTEM_H


#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "PredefinedProcedure.h"
#include "data/ast/ArrayTypeNode.h"
#include "data/symtab/SymbolTable.h"

using std::pair;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

class OberonSystem {

protected:
    virtual void initSymbolTable(SymbolTable *) = 0;

public:
    OberonSystem() = default;
    virtual ~OberonSystem();
    
    void createNamespace(const string &);
    void leaveNamespace();

    void createBasicTypes(const vector<pair<pair<TypeKind, unsigned int>, bool>>&);
    BasicTypeNode *createBasicType(TypeKind, unsigned);
    BasicTypeNode *getBasicType(TypeKind);

    PointerTypeNode *createPointerType(TypeNode *);

    ArrayTypeNode *createArrayType(const vector<unsigned> &, const vector<TypeNode *> &);

    PredefinedProcedure* createProcedure(ProcKind, const string &, const vector<pair<TypeNode *, bool>> &,
                                         TypeNode *, bool, bool);

    SymbolTable *getSymbolTable();

private:
    unique_ptr<SymbolTable> symbols_;
    vector<unique_ptr<DeclarationNode>> decls_;
    vector<unique_ptr<TypeNode>> types_;
    unordered_map<string, BasicTypeNode *> baseTypes_;
    string module_;

    TypeDeclarationNode *createTypeDeclaration(TypeNode *);

};


class Oberon07 final : public OberonSystem {

protected:
    void initSymbolTable(SymbolTable *) override;

public:
    explicit Oberon07() : OberonSystem() {}
    ~Oberon07() override = default;

};


#endif //OBERON_LANG_OBERONSYSTEM_H
