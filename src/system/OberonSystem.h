//
// Created by Michael Grossniklaus on 10/23/22.
//

#ifndef OBERON_LANG_OBERONSYSTEM_H
#define OBERON_LANG_OBERONSYSTEM_H


#include "data/symtab/SymbolTable.h"
#include "PredefinedProcedure.h"

class OberonSystem {

private:
    std::unique_ptr<SymbolTable> symbols_;
    std::vector<std::unique_ptr<Node>> predefines_;
    std::unordered_map<std::string, BasicTypeNode *> baseTypes_;
    std::unordered_map<std::string, BasicTypeNode *> procedures_;

protected:
    virtual void initSymbolTable(SymbolTable *symbols) = 0;

public:
    explicit OberonSystem() : symbols_(), predefines_(), baseTypes_() {};
    virtual ~OberonSystem();

    void createBasicTypes(std::vector<std::pair<std::pair<TypeKind, unsigned int>, bool>> types);
    BasicTypeNode *createBasicType(TypeKind kind, unsigned int size);
    BasicTypeNode *getBasicType(TypeKind kind);
    PointerTypeNode *createPointerType(TypeNode *base);
    void createProcedure(ProcType type, std::string name, std::vector<std::pair<TypeNode *, bool>> params,
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
