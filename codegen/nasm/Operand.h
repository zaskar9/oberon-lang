/*
 * Operands used by the instructions in the NASM file format.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_OPERAND_H
#define OBERON0C_OPERAND_H


#include <map>
#include <string>

enum class OpMode : unsigned char {
    q64 = 64, d32 = 32, w16 = 16, b8 = 8
};

class Operand {

public:
    virtual ~Operand() = default;
};

class Label : public Operand {

private:
    std::string name_;

public:
    explicit Label(std::string name) : name_(name) { };
    ~Label() override = default;

    const std::string getName() const;
};

class Register final : public Operand {

private:
    int id_;
    std::map<OpMode, std::string> names_;

public:
    explicit Register(int id, std::string n64, std::string n32, std::string n16, std::string n8) : id_(id),
            names_( { {OpMode::q64, n64}, {OpMode::d32, n32}, {OpMode::w16, n16}, {OpMode::b8, n8} } ) { };
    ~Register() override = default;

    int getId() const;
    const std::string getName() const;
    const std::string getName(OpMode mode) const;
};

class Immediate final : public Operand {

private:
    int value_;

public:
    explicit Immediate(int value) : value_(value) { };
    ~Immediate() override = default;

    int getValue() const;
};

class Address : public Operand {

public:
    ~Address() override = default;
};

class EffectiveAddress final : public Address {

private:
    const Register* base_;
    const int displacement_;
    const Register* index_;
    const int scale_;

public:
    explicit EffectiveAddress(const Register* base, const int displacement, const Register* index, const int scale) :
            base_(base), displacement_(displacement), index_(index), scale_(scale) { };
    explicit EffectiveAddress(const Register* base) : base_(base), displacement_(0), index_(), scale_(1) { };
    ~EffectiveAddress() override = default;

    const Register* getBase() const;
    int getDisplacement() const;
    const Register* getIndex() const;
    int getScale() const;
};

class LabelAddress final : public Address {

private:
    const Label* label_;

public:
    explicit LabelAddress(const Label* label) : label_(label) { };
    ~LabelAddress() override = default;

    const Label* getLabel() const;
};

#endif //OBERON0C_OPERAND_H
