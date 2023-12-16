//
// Created by Michael Grossniklaus on 10/23/22.
//

#ifndef OBERON_LANG_PREDEFINEDPROCEDURE_H
#define OBERON_LANG_PREDEFINEDPROCEDURE_H


#include "data/ast/ProcedureNode.h"

enum class ProcType {
    ABS, ASH, ASR, ASSERT, CAP, CHR, COPY, DEC, ENTIER, EXCL, FLOOR, FLT, FREE, HALT, INC, INCL, LEN, LONG, LSL,
    MAX, MIN, NEW, ODD, ORD, PACK, ROL, ROR, SHORT, SIZE, UNPK
};

class PredefinedProcedure final : public ProcedureNode {

private:
    ProcType type_;

public:
    explicit PredefinedProcedure(ProcType type, std::string name, std::vector<std::pair<TypeNode*, bool>> params, TypeNode *ret);
    ~PredefinedProcedure() override;

    [[nodiscard]] ProcType getProcType() const;

    [[nodiscard]] bool isPredefined() const override {
        return true;
    }

};


#endif //OBERON_LANG_PREDEFINEDPROCEDURE_H
