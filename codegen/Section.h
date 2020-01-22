#include <utility>

/*
 * Header of the NASM assembly sections used code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_SECTION_H
#define OBERON0C_SECTION_H


#include <ostream>
#include <string>
#include <unordered_map>
#include "BasicBlock.h"

class Section {

private:
    std::string comment_;

public:
    explicit Section() : comment_() { };
    virtual ~Section() = default;

    void setComment(const std::string& comment);
    const std::string getComment();
};


class DataSection final : Section {

    friend std::ostream& operator<<(std::ostream &stream, const DataSection &section);

};


class BssSection final : Section {

private:
    std::unordered_map<std::unique_ptr<Label>, int> labels_;

public:
    explicit BssSection() : labels_() { };
    ~BssSection() override = default;

    Label* reserveBytes(const std::string &label, int num);

    friend std::ostream& operator<<(std::ostream &stream, const BssSection &section);

};

class TextSection final : Section {

private:
    std::vector<std::unique_ptr<BasicBlock>> blocks_;

public:
    explicit TextSection() : blocks_() { };
    ~TextSection() override = default;

    BasicBlock* addBasicBlock(const std::string &label, const std::string &comment);

    friend std::ostream& operator<<(std::ostream &stream, const TextSection &section);

};

#endif //OBERON0C_SECTION_H
