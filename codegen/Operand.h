/*
 * Header of the arguments used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_OPERAND_H
#define OBERON0C_OPERAND_H


#include <string>

class Operand {

public:
    virtual ~Operand() = default;
};


class Label final : Operand {

private:
    std::string name_;

public:
    explicit Label(std::string name) : name_(std::move(name)) { };
    ~Label() override = default;

    const std::string getName() const;

};


class Register final : Operand {

};


class Immediate final : Operand {

private:
    int value_;

public:
    explicit Immediate(int value) : value_(value) { };
    ~Immediate() override = default;

    const int getValue() const;

};

#endif //OBERON0C_OPERAND_H
