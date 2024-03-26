//
// Created by Michael Grossniklaus on 10/23/22.
//

#ifndef OBERON_LANG_OBERONSYSTEM_H
#define OBERON_LANG_OBERONSYSTEM_H


#include "data/ast/ArrayTypeNode.h"
#include "data/symtab/SymbolTable.h"
#include "PredefinedProcedure.h"

class OberonSystem {

private:
    std::unique_ptr<SymbolTable> symbols_;
    std::vector<std::unique_ptr<DeclarationNode>> decls_;
    std::vector<std::unique_ptr<TypeNode>> types_;
    std::unordered_map<std::string, BasicTypeNode *> baseTypes_;
    std::string module_;

    TypeDeclarationNode *createTypeDeclaration(TypeNode *);


protected:
    virtual void initSymbolTable(SymbolTable *) = 0;

public:
    explicit OberonSystem() : symbols_(), decls_(), types_(), baseTypes_() {};
    virtual ~OberonSystem();
    
    void createNamespace(const std::string &module);
    void leaveNamespace();

    void createBasicTypes(const std::vector<std::pair<std::pair<TypeKind, unsigned int>, bool>>&);
    BasicTypeNode *createBasicType(TypeKind, unsigned);
    BasicTypeNode *getBasicType(TypeKind);

    PointerTypeNode *createPointerType(TypeNode *);

    ArrayTypeNode *createArrayType(const vector<unsigned> &, const vector<TypeNode *> &);

    PredefinedProcedure* createProcedure(ProcKind, const std::string &,
                                         const std::vector<std::pair<TypeNode *, bool>> &,
                                         TypeNode *, bool, bool);

    SymbolTable *getSymbolTable();

};


class Oberon07 final : public OberonSystem {

protected:
    void initSymbolTable(SymbolTable *) override;

public:
    explicit Oberon07() : OberonSystem() {};
    ~Oberon07() override = default;

};


#endif //OBERON_LANG_OBERONSYSTEM_H
