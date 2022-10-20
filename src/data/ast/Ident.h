//
// Created by Michael Grossniklaus on 3/6/22.
//

#ifndef OBERON_LANG_IDENT_H
#define OBERON_LANG_IDENT_H


#include <optional>
#include <string>
#include <utility>
#include "Node.h"
#include "../../global.h"

class Ident {

private:
    FilePos pos_;
    std::string name_;

public:
    explicit Ident(std::string name) : pos_(EMPTY_POS), name_(std::move(name)) { };
    explicit Ident(const FilePos &pos, std::string name) : pos_(pos), name_(std::move(name)) { };
    explicit Ident(Ident *ident) : pos_(ident->pos_), name_(ident->name_) { };
    virtual ~Ident();

    [[nodiscard]] FilePos pos() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] virtual bool isQualified() const { return false; };
    [[nodiscard]] virtual bool isExported() const { return false; };

    virtual void print(std::ostream &stream) const;
    [[nodiscard]] virtual bool equals(const Ident &other) const;

    friend std::ostream& operator<<(std::ostream &stream, const Ident &ident);
    friend bool operator==(const Ident &a, const Ident &b);
    friend bool operator!=(const Ident &a, const Ident &b);

};

class IdentDef final : public Ident {

private:
    bool exported_;

public:
    explicit IdentDef(std::string name, bool exported = false) :
            Ident(std::move(name)), exported_(exported) { };
    explicit IdentDef(const FilePos &pos, std::string name, bool exported = false) :
            Ident(pos, std::move(name)), exported_(exported) { };
    ~IdentDef() override;

    [[nodiscard]] bool isExported() const override;

};

class QualIdent final : public Ident {

private:
    std::optional<std::string> qualifier_;

    explicit QualIdent(const FilePos &pos, std::optional<std::string> qualifier, std::string name) :
            Ident(pos, name), qualifier_(std::move(qualifier)) {};

public:
    explicit QualIdent(std::string name) :
            Ident(std::move(name)), qualifier_(std::nullopt) {};
    explicit QualIdent(std::string qualifier, std::string name) :
            Ident(name), qualifier_(std::optional<std::string>(std::move(qualifier))) {};
    explicit QualIdent(const FilePos &pos, std::string name) :
            Ident(pos, std::move(name)), qualifier_(std::nullopt) {};
    explicit QualIdent(const FilePos &pos, std::string qualifier, std::string name) :
            Ident(pos, name), qualifier_(std::optional<std::string>(std::move(qualifier))) {};
    explicit QualIdent(Ident *ident) :
            Ident(ident), qualifier_(ident->isQualified() ? dynamic_cast<QualIdent *>(ident)->qualifier_ : std::nullopt) {};
    ~QualIdent() override;

    [[nodiscard]] bool isQualified() const override;

    [[nodiscard]] std::string qualifier() const;

    void print(std::ostream &stream) const override;
    bool equals(const Ident &other) const override;

};


#endif //OBERON_LANG_IDENT_H
