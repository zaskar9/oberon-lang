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

    TypeDeclarationNode *createTypeDeclaration(TypeNode *);


protected:
    virtual void initSymbolTable(SymbolTable *symbols) = 0;

public:
    explicit OberonSystem() : symbols_(), decls_(), types_(), baseTypes_() {};
    virtual ~OberonSystem();

    void createBasicTypes(std::vector<std::pair<std::pair<TypeKind, unsigned int>, bool>> types);
    BasicTypeNode *createBasicType(TypeKind kind, unsigned int size);
    BasicTypeNode *getBasicType(TypeKind kind);

    PointerTypeNode *createPointerType(TypeNode *base);

    ArrayTypeNode *createArrayType(TypeNode *memberType, unsigned int dimension);

    void createProcedure(ProcKind type, const std::string& name, std::vector<std::pair<TypeNode *, bool>> params,
                         TypeNode *ret, bool hasVarArgs, bool toSymbols);

    SymbolTable *getSymbolTable();

};


class Oberon07 final : public OberonSystem {

protected:
    void initSymbolTable(SymbolTable *symbols) override;

public:
    explicit Oberon07() : OberonSystem() {};
    ~Oberon07() override = default;

};


#endif //OBERON_LANG_OBERONSYSTEM_H
