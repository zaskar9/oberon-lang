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
ASTContext::getOrInsertArrayType(const FilePos &start, const FilePos &,
                                 unsigned dimensions, vector<unsigned> lengths, vector<TypeNode *> types, ModuleNode *module) {
    // compute the (logical) memory size of this array type
    unsigned size = 1;
    for (unsigned length : lengths) {
        size *= length;
    }
    size *= types[types.size() - 1]->getSize();
    auto type = make_unique<ArrayTypeNode>(start, dimensions, std::move(lengths), std::move(types));
    type->setModule(module ? module : module_.get());
    type->setSize(size);
    auto res = type.get();
    // cache the new array type
    array_ts_.push_back(std::move(type));
    return res;
}

RecordTypeNode *
ASTContext::getOrInsertRecordType(const FilePos &start, const FilePos &,
                                  RecordTypeNode *base, vector<unique_ptr<FieldNode>> fields, ModuleNode *module) {
    auto type = make_unique<RecordTypeNode>(start, base, std::move(fields));
    type->setModule(module ? module : module_.get());
    auto res = type.get();
    record_ts_.push_back(std::move(type));
    return res;
}

PointerTypeNode *
ASTContext::getOrInsertPointerType(const FilePos &start, const FilePos &, TypeNode *base, ModuleNode *module) {
    auto type = make_unique<PointerTypeNode>(start, base);
    type->setModule(module ? module : module_.get());
    auto res = type.get();
    pointer_ts_.push_back(std::move(type));
    return res;
}

ProcedureTypeNode *
ASTContext::getOrInsertProcedureType(const FilePos &start, const FilePos &,
                                     vector<unique_ptr<ParameterNode>> params, bool varargs, TypeNode *ret, ModuleNode *module) {
    auto type = make_unique<ProcedureTypeNode>(start, std::move(params), varargs, ret);
    type->setModule(module ? module : module_.get());
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

void ASTContext::addExternalProcedure(ProcedureDeclarationNode *proc) {
    if (std::find(ext_procedures_.begin(), ext_procedures_.end(), proc) == ext_procedures_.end()) {
        ext_procedures_.push_back(proc);
    }
}

ProcedureDeclarationNode *ASTContext::getExternalProcedure(const size_t num) const {
    return ext_procedures_.at(num);
}

size_t ASTContext::getExternalProcedureCount() const {
    return ext_procedures_.size();
}