//
// Created by Michael Grossniklaus on 3/6/22.
//

#include "Ident.h"

Ident::~Ident() = default;

FilePos Ident::pos() const {
    return pos_;
}

std::string Ident::name() const {
    return name_;
}

void Ident::print(std::ostream &stream) const {
    stream << name_ << (this->isExported() ? "*" : "");
}

bool Ident::equals(const Ident &other) const {
    return this->name_ == other.name_;
}

std::ostream& operator<<(std::ostream &stream, const Ident &ident) {
    ident.print(stream);
    return stream;
}

bool operator==(const Ident &a, const Ident &b) {
    return a.equals(b);
}

bool operator!=(const Ident &a, const Ident &b) {
    return !(a == b);
}

IdentDef::~IdentDef() = default;

bool IdentDef::isExported() const {
    return exported_;
}

QualIdent::~QualIdent() = default;

bool QualIdent::isQualified() const {
    return qualifier_.has_value();
}

std::string QualIdent::qualifier() const {
    return qualifier_.value();
}

void QualIdent::print(std::ostream &stream) const {
    if (this->isQualified()) {
        stream << this->qualifier() << ".";
    }
    stream << this->name();
}

bool QualIdent::equals(const Ident &other) const {
    if (other.isQualified()) {
        if (this->isQualified()) {
            return this->qualifier() == dynamic_cast<const QualIdent*>(&other)->qualifier();
        } else {
            return false;
        }
    } else {
        if (this->isQualified()) {
            return false;
        } else {
            return this->name() == other.name();
        }
    }
}
