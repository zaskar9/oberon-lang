//
// Created by Michael Grossniklaus on 3/6/22.
//

#ifndef OBERON_LANG_IDENT_H
#define OBERON_LANG_IDENT_H


#include "global.h"
#include "Node.h"
#include <optional>
#include <string>
#include <utility>

using std::optional;
using std::string;

class Ident {

private:
    FilePos start_;
    FilePos end_;
    string name_;

public:
    explicit Ident(const string &name) : start_(EMPTY_POS), end_(EMPTY_POS), name_(name) {};
    Ident(const FilePos &start, const FilePos &end, string name) : start_(start), end_(end), name_(std::move(name)) {};
    explicit Ident(Ident *ident) : start_(ident->start_), end_(ident->end_), name_(ident->name_) {};
    virtual ~Ident();

    [[nodiscard]] FilePos start() const;
    [[nodiscard]] FilePos end() const;
    [[nodiscard]] string name() const;
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
    explicit IdentDef(const string &name, bool exported = false) :
            Ident(name), exported_(exported) { };
    explicit IdentDef(const FilePos &start, const FilePos &end, const string &name, bool exported = false) :
            Ident(start, end, name), exported_(exported) { };
    ~IdentDef() override;

    [[nodiscard]] bool isExported() const override;

};

class QualIdent final : public Ident {

private:
    optional<string> qualifier_;

    explicit QualIdent(const FilePos &start, const FilePos &end,  optional<string> qualifier, const string &name) :
            Ident(start, end, name), qualifier_(std::move(qualifier)) {};

public:
    explicit QualIdent(const string &name) :
            Ident(name), qualifier_(std::nullopt) {};
    explicit QualIdent(const string &qualifier, const string &name) :
            Ident(name), qualifier_(optional<string>(qualifier)) {};
    explicit QualIdent(const FilePos &start, const FilePos &end, const string &name) :
            Ident(start, end, name), qualifier_(std::nullopt) {};
    explicit QualIdent(const FilePos &start, const FilePos &end, const string &qualifier, const string &name) :
            Ident(start, end, name), qualifier_(optional<string>(std::move(qualifier))) {};
    explicit QualIdent(Ident *ident) :
            Ident(ident), qualifier_(ident->isQualified() ? dynamic_cast<QualIdent *>(ident)->qualifier_ : std::nullopt) {};
    ~QualIdent() override;

    [[nodiscard]] bool isQualified() const override;

    [[nodiscard]] string qualifier() const;

    void print(std::ostream &stream) const override;
    [[nodiscard]] bool equals(const Ident &other) const override;

};


#endif //OBERON_LANG_IDENT_H
