//
// Created by Michael Grossniklaus on 1/28/24.
//

#include "ASTContext.h"
#include <algorithm>
#include <memory>

using std::filesystem::path;
using std::make_unique;

const path &
ASTContext::getSourceFileName() const {
    return file_;
}

ModuleNode *
ASTContext::getTranslationUnit() const {
    return module_.get();
}

void
ASTContext::setTranslationUnit(unique_ptr<ModuleNode> module) {
    module_ = std::move(module);
}

ArrayTypeNode *
ASTContext::getOrInsertArrayType(const FilePos &start, const FilePos &end,
                                 unsigned length, TypeNode *memberType) {
    return getOrInsertArrayType(start, end, 1, { length }, { memberType });
}

ArrayTypeNode *
ASTContext::getOrInsertArrayType(const FilePos &start, [[maybe_unused]] const FilePos &end,
                                 unsigned dimensions, vector<unsigned> lengths, vector<TypeNode *> types) {
    for (auto &type : array_ts_) {
        if (type->dimensions() == dimensions) {
            bool found = true;
            for (unsigned i = 0; i < dimensions; i++) {
                if (lengths[i] != type->lengths()[i] || types[i] != type->types()[i]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return type.get();
            }
        }
    }
    unsigned size = 1;
    for (unsigned length : lengths) {
        size *= length;
    }
    size *= types[types.size() - 1]->getSize();
    auto type = make_unique<ArrayTypeNode>(start, dimensions, std::move(lengths), std::move(types));
    type->setSize(size);
    auto res = type.get();
    array_ts_.push_back(std::move(type));
    return res;
}

RecordTypeNode *
ASTContext::getOrInsertRecordType(const FilePos &start, [[maybe_unused]] const FilePos &end,
                                  vector<unique_ptr<FieldNode>> fields) {
    auto type = make_unique<RecordTypeNode>(start, std::move(fields));
    auto res = type.get();
    record_ts_.push_back(std::move(type));
    return res;
}

PointerTypeNode *
ASTContext::getOrInsertPointerType(const FilePos &start, [[maybe_unused]] const FilePos &end, TypeNode *base) {
    auto type = make_unique<PointerTypeNode>(start, base);
    auto res = type.get();
    pointer_ts_.push_back(std::move(type));
    return res;
}

ProcedureTypeNode *
ASTContext::getOrInsertProcedureType(const FilePos &start, [[maybe_unused]] const FilePos &end,
                                     vector<unique_ptr<ParameterNode>> params, bool varargs, TypeNode *ret) {
    auto type = make_unique<ProcedureTypeNode>(start, std::move(params), varargs, ret);
    auto res = type.get();
    procedure_ts.push_back(std::move(type));
    return res;
}

void ASTContext::addExternalModule(std::unique_ptr<ModuleNode> module) {
    string name = module->getIdentifier()->name();
    ext_modules_[name] = std::move(module);
}

ModuleNode *ASTContext::getExternalModule(const std::string &name) {
    return ext_modules_[name].get();
}

void ASTContext::addExternalProcedure(ProcedureNode *proc) {
    if (std::find(ext_procedures_.begin(), ext_procedures_.end(), proc) == ext_procedures_.end()) {
        ext_procedures_.push_back(proc);
    }
}

ProcedureNode *ASTContext::getExternalProcedure(size_t num) const {
    return ext_procedures_.at(num);
}

size_t ASTContext::getExternalProcedureCount() const {
    return ext_procedures_.size();
}