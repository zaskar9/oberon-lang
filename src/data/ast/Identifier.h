//
// Created by Michael Grossniklaus on 3/6/22.
//

#ifndef OBERON_LANG_IDENTIFIER_H
#define OBERON_LANG_IDENTIFIER_H


#include <string>
#include <utility>
#include "../../global.h"

class Identifier {

private:
    FilePos pos_;
    std::string name_;
    std::string qualifier_;
    bool exported_;

public:
    explicit Identifier(std::string name, bool exported = false) :
            pos_(EMPTY_POS), name_(std::move(name)), qualifier_(), exported_(exported) { };
    explicit Identifier(std::string qualifier, std::string name, bool exported = false) :
            pos_(EMPTY_POS), name_(std::move(name)), qualifier_(std::move(qualifier)), exported_(exported) { };
    explicit Identifier(const FilePos &pos, std::string name, bool exported = false) :
            pos_(pos), name_(std::move(name)), qualifier_(), exported_(exported) { };
    explicit Identifier(const FilePos &pos, std::string qualifier, std::string name) :
            pos_(pos), name_(std::move(name)), qualifier_(std::move(qualifier)), exported_(false) { };
    explicit Identifier(Identifier* ident) :
            pos_(ident->pos_), name_(ident->name_), qualifier_(ident->qualifier_), exported_(ident->exported_) { };
    ~Identifier() = default;

    [[nodiscard]] FilePos pos() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] std::string qualifier() const;
    [[nodiscard]] bool isQualified() const;
    [[nodiscard]] bool isExported() const;

    friend std::ostream& operator<<(std::ostream &stream, const Identifier &ident);
    friend bool operator==(const Identifier &a, const Identifier &b);
    friend bool operator!=(const Identifier &a, const Identifier &b);

};


#endif //OBERON_LANG_IDENTIFIER_H
