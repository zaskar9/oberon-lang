//
// Created by Michael Grossniklaus on 3/6/22.
//

#include "Identifier.h"

FilePos Identifier::pos() const {
    return pos_;
}

std::string Identifier::name() const {
    return name_;
}

std::string Identifier::qualifier() const {
    return qualifier_;
}

bool Identifier::isQualified() const {
    return !qualifier_.empty();
}

bool Identifier::isExported() const {
    return exported_;
}

std::ostream& operator<<(std::ostream &stream, const Identifier &ident) {
    if (!ident.qualifier_.empty()) {
        stream << ident.qualifier_ << std::string(".");
    }
    stream << ident.name_;
    if (ident.exported_) {
        stream << std::string("*");
    }
    return stream;
}

bool operator==(const Identifier &a, const Identifier &b) {
    return a.name_ == b.name_ && a.qualifier_ == b.qualifier_;
}

bool operator!=(const Identifier &a, const Identifier &b) {
    return !(a == b);
}