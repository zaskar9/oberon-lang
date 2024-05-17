//
// Created by Michael Grossniklaus on 10/23/22.
//

#ifndef OBERON_LANG_PREDEFINEDPROCEDURE_H
#define OBERON_LANG_PREDEFINEDPROCEDURE_H


#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "data/ast/ProcedureNode.h"

using std::make_unique;
using std::pair;
using std::string;
using std::unique_ptr;
using std::vector;

enum class ProcKind {
    ABS, ASH, ASR, ASSERT, CAP, CHR, COPY, DEC, ENTIER, EXCL, FLOOR, FLT, DISPOSE, HALT, INC, INCL, LEN, LONG, LSL,
    MAX, MIN, NEW, ODD, ORD, PACK, ROR, SHORT, SIZE, UNPK,
    SYSTEM_ADR, SYSTEM_GET, SYSTEM_PUT, SYSTEM_SIZE, SYSTEM_BIT, SYSTEM_COPY, SYSTEM_VAL
};

class PredefinedProcedure : public ProcedureNode {

private:
    vector<unique_ptr<ProcedureTypeNode>> types_;
    bool isCast_;
    ProcKind kind_;

    int matchType(TypeNode*, TypeNode*) const;

public:
    explicit PredefinedProcedure(ProcKind, const string &, const vector<pair<TypeNode*, bool>> &, bool, TypeNode *);
    ~PredefinedProcedure() override;

    ProcedureTypeNode* overload(const vector<pair<TypeNode*, bool>> &, bool, TypeNode *);
    ProcedureTypeNode* dispatch(vector<TypeNode*>, TypeNode *);
    [[nodiscard]] bool isOverloaded() const;

    [[nodiscard]] ProcKind getKind() const;

    [[nodiscard]] bool isPredefined() const override {
        return true;
    }

};


#endif //OBERON_LANG_PREDEFINEDPROCEDURE_H
