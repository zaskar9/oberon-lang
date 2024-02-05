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
    ABS, ASH, ASR, ASSERT, CAP, CHR, COPY, DEC, ENTIER, EXCL, FLOOR, FLT, FREE, HALT, INC, INCL, LEN, LONG, LSL,
    MAX, MIN, NEW, ODD, ORD, PACK, ROL, ROR, SHORT, SIZE, UNPK
};

class PredefinedProcedure final : public ProcedureNode {

private:
    unique_ptr<ProcedureTypeNode> type_;
    ProcKind kind_;

public:
    explicit PredefinedProcedure(ProcKind, const string &, const vector<pair<TypeNode*, bool>> &, TypeNode *);
    ~PredefinedProcedure() override;

    [[nodiscard]] ProcKind getKind() const;

    [[nodiscard]] bool isPredefined() const override {
        return true;
    }

};


#endif //OBERON_LANG_PREDEFINEDPROCEDURE_H
