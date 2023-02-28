//
// Created by Michael Grossniklaus on 10/23/22.
//

#ifndef OBERON_LANG_PREDEFINEDPROCEDURE_H
#define OBERON_LANG_PREDEFINEDPROCEDURE_H


#include <llvm/IR/IRBuilder.h>
#include "OberonSystem.h"
#include "data/ast/NodeReference.h"

using namespace llvm;

class PredefinedProcedure : public ProcedureNode {

protected:
    void setSignature(std::vector<std::pair<TypeNode*, bool>> params, TypeNode *ret);

public:
    explicit PredefinedProcedure(std::string name) : ProcedureNode(EMPTY_POS, std::make_unique<Ident>(name)) {};
    ~PredefinedProcedure() override;

    virtual void setup(OberonSystem *system) = 0;

    [[nodiscard]] bool isPredefined() const override {
        return true;
    }

};


class New final : public PredefinedProcedure {

public:
    explicit New() : PredefinedProcedure(New::NAME) {};
    ~New() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};

class Free final : public PredefinedProcedure {

public:
    explicit Free() : PredefinedProcedure(Free::NAME) {}
    ~Free() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Inc final : public PredefinedProcedure {

public:
    explicit Inc() : PredefinedProcedure(Inc::NAME) {};
    ~Inc() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Dec final : public PredefinedProcedure {

public:
    explicit Dec() : PredefinedProcedure(Dec::NAME) {};
    ~Dec() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Lsl final : public PredefinedProcedure {

public:
    explicit Lsl() : PredefinedProcedure(Lsl::NAME) {};
    ~Lsl() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Asr final : public PredefinedProcedure {

public:
    explicit Asr() : PredefinedProcedure(Asr::NAME) {};
    ~Asr() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Ror final : public PredefinedProcedure {

public:
    explicit Ror() : PredefinedProcedure(Ror::NAME) {};
    ~Ror() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Rol final : public PredefinedProcedure {

public:
    explicit Rol() : PredefinedProcedure(Rol::NAME) {};
    ~Rol() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Odd final : public PredefinedProcedure {

public:
    explicit Odd() : PredefinedProcedure(Odd::NAME) {};
    ~Odd() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Halt final : public PredefinedProcedure {

public:
    explicit Halt() : PredefinedProcedure(Halt::NAME) {};
    ~Halt() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


class Assert final : public PredefinedProcedure {
public:
    explicit Assert() : PredefinedProcedure(Assert::NAME) {};
    ~Assert() override = default;

    void setup(OberonSystem *system) override;

    static const std::string NAME;

};


#endif //OBERON_LANG_PREDEFINEDPROCEDURE_H
