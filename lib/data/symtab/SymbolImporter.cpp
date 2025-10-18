//
// Created by Michael Grossniklaus on 3/19/22.
//

#include "SymbolImporter.h"

#include <bitset>
#include <filesystem>
#include <memory>
#include <string>

#include "SymbolFile.h"
#include "data/ast/NodePrettyPrinter.h"

using std::bitset;
using std::make_unique;
using std::string;
using std::unique_ptr;

ModuleNode *SymbolImporter::read(const string &name) {
    const auto path = context_.getSourceFileName().parent_path();
    auto fp = (path / name).replace_extension(".smb");
    const auto include = fp.filename();
    if (!std::filesystem::exists(fp)) {
        logger_.debug("Symbol file not found: '" + fp.string() + "'.");
        const auto opt = config_.findInclude(include);
        if (opt.has_value()) {
            fp = opt.value();
        } else {
            logger_.debug("Symbol file not found: '" + include.string() + "'.");
            return nullptr;
        }
    }
    logger_.debug("Symbol file found: '" + fp.string() + "'.");
    const auto file = make_unique<SymbolFile>();
    file->open(fp.string(), std::ios::in);

    // read symbol file header
    [[maybe_unused]] auto key = file->readLong();
    [[maybe_unused]] auto ident = file->readString();
    const auto version = file->readChar();
    if (version != SymbolFile::VERSION) {
        logger_.error(fp.string(), "Wrong symbol file version: expected " + to_string(SymbolFile::VERSION) +
                                   ", found " + to_string(static_cast<int>(version)) + ".");
        return nullptr;
    }
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    // get or create module
    module_ = getOrCreateModule(name);
    auto ch = file->readChar();
    while (ch != 0 && !file->eof()) {
        const auto nodeType = static_cast<NodeType>(ch);
        readDeclaration(file.get(), nodeType);
#ifdef _DEBUG
        std::cout << std::endl;
#endif
        // check for terminator
        ch = file->readChar();
    }
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    xrefs_.clear();
    if (!fwds_.empty()) {
        logger_.error(fp.string(), "Unresolved forward type references during import.");
        fwds_.clear();
    }
    file->flush();
    file->close();
#ifdef _DEBUG
    const auto printer = make_unique<NodePrettyPrinter>(std::cout);
    printer->print(module_);
#endif
    return module_;
}

void SymbolImporter::readDeclaration(SymbolFile *file, const NodeType nodeType) {
    // read the symbol name
    auto name = file->readString();
    auto ident = make_unique<IdentDef>(name);
    // check whether symbol already exists
    const auto module = module_->getIdentifier()->name();
    const auto decl = dynamic_cast<TypeDeclarationNode *>(symbols_->lookup(module, name));
    // read symbol type
    auto type = readType(file, decl);
    // create symbol
    if (nodeType == NodeType::constant) {
        // constant declaration
        const auto kind = type->kind();
        if (kind == TypeKind::PROCEDURE) {
            // read in export number
            [[maybe_unused]] auto exno = file->readChar();
        } else {
            std::unique_ptr<ExpressionNode> expr;
            switch (kind) {
                case TypeKind::STRING:
                    expr = make_unique<StringLiteralNode>(EMPTY_POS, file->readString(), type);
                    break;
                case TypeKind::BOOLEAN:
                    expr = make_unique<BooleanLiteralNode>(EMPTY_POS, file->readChar() != 0, type);
                    break;
                case TypeKind::CHAR:
                    expr = make_unique<CharLiteralNode>(EMPTY_POS, static_cast<unsigned char>(file->readChar()), type);
                    break;
                case TypeKind::SHORTINT:
                    expr = make_unique<IntegerLiteralNode>(EMPTY_POS, file->readShort(), type);
                    break;
                case TypeKind::INTEGER:
                    expr = make_unique<IntegerLiteralNode>(EMPTY_POS, file->readInt(), type);
                    break;
                case TypeKind::LONGINT:
                    expr = make_unique<IntegerLiteralNode>(EMPTY_POS, file->readLong(), type);
                    break;
                case TypeKind::REAL:
                    expr = make_unique<RealLiteralNode>(EMPTY_POS, file->readFloat(), type);
                    break;
                case TypeKind::LONGREAL:
                    expr = make_unique<RealLiteralNode>(EMPTY_POS, file->readDouble(), type);
                    break;
                case TypeKind::SET:
                    expr = make_unique<SetLiteralNode>(EMPTY_POS, bitset<32>(static_cast<unsigned>(file->readInt())), type);
                    break;
                default:
                    logger_.error(file->path(), "Cannot import constant " + name + ".");
            }
            if (expr) {
                auto node = make_unique<ConstantDeclarationNode>(std::move(ident), std::move(expr));
                symbols_->import(module_->getIdentifier()->name(), name, node.get());
                node->setModule(module_);
                module_->constants().push_back(std::move(node));
            }
        }
    } else if (nodeType == NodeType::type && !decl) {
        // type declaration
        auto node = make_unique<TypeDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module_->getIdentifier()->name(), name, node.get());
        node->setModule(module_);
        module_->types().push_back(std::move(node));
    } else if (nodeType == NodeType::variable) {
        // variable declaration
        [[maybe_unused]] auto exno = file->readChar();  // read in export number
        auto node = make_unique<VariableDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module_->getIdentifier()->name(), name, node.get());
        node->setModule(module_);
        module_->variables().push_back(std::move(node));
    } else if (nodeType == NodeType::procedure) {
        // procedure declaration
        auto node = make_unique<ProcedureDeclarationNode>(std::move(ident), dynamic_cast<ProcedureTypeNode *>(type));
        symbols_->import(module_->getIdentifier()->name(), name, node.get());
        node->setModule(module_);
        module_->procedures().push_back(std::move(node));
    }
}

TypeNode *SymbolImporter::readType(SymbolFile *file, const TypeDeclarationNode *decl, PointerTypeNode *ptr) {
    TypeNode *type = nullptr;
    const auto ch = file->readChar();
    // handle type references
    unsigned ref = 0;
    if (ch < 0) {
        ref = static_cast<unsigned char>(-ch);
        if (ref == static_cast<char>(TypeKind::NOTYPE)) {
            return system_.getBasicType(TypeKind::NOTYPE);
        }
        // check whether the referenced type has already been imported
        type = this->getXRef(ref);
        if (type) {
            return type;
        }
        // referenced type has not yet been imported, create a forward reference
        if (ptr) {
            fwds_[ref] = ptr;
            return nullptr;
        }
        logger_.error(file->path(), "Unresolved type reference during import: " + to_string(ref) + ".");
        return nullptr;
    }
    // handle re-exported types
    string module, name;
    if (ch > 0) {
        ref = static_cast<unsigned char>(ch);
        module = file->readString();
        if (!module.empty()) {
            // check whether re-exported type has already been imported
            name = file->readString();
            decl = dynamic_cast<TypeDeclarationNode *>(symbols_->lookup(module, name));
        }
    }
    // read, possibly redundant, type description
    const auto kind = static_cast<TypeKind>(file->readChar());
    if (kind == TypeKind::ARRAY) {
        type = readArrayType(file);
    } else if (kind == TypeKind::POINTER) {
        type = readPointerType(file, ref);
    } else if (kind == TypeKind::PROCEDURE) {
        type = readProcedureType(file);
    } else if (kind == TypeKind::RECORD) {
        type = readRecordType(file);
    }
    if (decl) {
        // discard just imported type, if it has already been imported previously
        type = decl->getType();
    } else if (type) {
        // import a re-exported
        if (!module.empty()) {
            const auto external = getOrCreateModule(module);
            auto node = make_unique<TypeDeclarationNode>(EMPTY_POS, make_unique<IdentDef>(name), type);
            symbols_->import(module, name, node.get());
            node->setModule(external);
            external->types().push_back(std::move(node));
        }
    } else {
        // abort no declaration found and no type imported
        return nullptr;
    }
    this->setXRef(ref, type);
    // check if the imported type resolves a forward reference
    if (fwds_.contains(ref)) {
        fwds_[ref]->setBase(type);
        fwds_.erase(ref);
    }
    return type;
}

TypeNode *SymbolImporter::readArrayType(SymbolFile *file) {
    unsigned dim = 1;
    // read member type
    vector<TypeNode *> types;
    TypeNode *type = readType(file);
    types.push_back(type);
    // read dimension
    vector<unsigned> lengths;
    const auto length = static_cast<unsigned>(file->readInt());
    lengths.push_back(length);
    // check for multi-dimensional array
    if (type->isArray()) {
        const auto array = dynamic_cast<ArrayTypeNode *>(type);
        dim += array->dimensions();
        lengths.insert(lengths.end(), array->lengths().begin(), array->lengths().end());
        types.insert(types.end(), array->types().begin(), array->types().end());
    }
    const auto res = context_.getOrInsertArrayType(EMPTY_POS, EMPTY_POS, dim, lengths, types, module_);
    // read in size
    res->setSize(static_cast<unsigned>(file->readInt()));
    return res;
}

TypeNode *SymbolImporter::readPointerType(SymbolFile *file, const unsigned ref) {
    // create a pointer type with null base type
    const auto ptr = context_.getOrInsertPointerType(EMPTY_POS, EMPTY_POS, nullptr, module_);
    this->setXRef(ref, ptr);
    // try to read and set the base type
    if (const auto base = readType(file, nullptr, ptr)) {
        ptr->setBase(base);
    }
    return ptr;
}

TypeNode *SymbolImporter::readProcedureType(SymbolFile *file) {
    // read return type
    TypeNode *ret = readType(file);
    // read parameters
    std::vector<std::unique_ptr<ParameterNode>> params;
    TypeNode *type = nullptr;
    unsigned index = 0;
    auto ch = file->readChar();
    while (ch != 0) {
        const auto var = file->readChar();
        if (const auto cur = readType(file); cur && cur->kind() != TypeKind::NOTYPE) {
            index = 0;
            type = cur;
        } else {
            index++;
        }
        auto param = make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>("_"), type, (var == 0), index);
        param->setScope(SymbolTable::MODULE_SCOPE);
        params.push_back(std::move(param));
        // check for terminator
        ch = file->readChar();
    }
    return context_.getOrInsertProcedureType(EMPTY_POS, EMPTY_POS, std::move(params), false, ret, module_);
}

TypeNode *SymbolImporter::readRecordType(SymbolFile *file) {
    // TODO add sanity checks based on the currently unused information
    // for extended records, read base type of record type (or TypeKind::NOTYPE)
    auto *base_t = dynamic_cast<RecordTypeNode*>(readType(file));
    // read export number
    [[maybe_unused]] auto exno = file->readChar();
    // read the number of fields in this record
    [[maybe_unused]] auto fld_cnt = file->readInt();
    // read the size of the type, i.e., sum of the sizes of the types of all fields
    const auto size = static_cast<unsigned>(file->readInt());
    // read fields
    vector<unique_ptr<FieldNode>> fields;
    TypeNode *type = nullptr;
    int index = 0;
    auto ch = file->readChar();
    while (ch != 0) {
        // read field number
        [[maybe_unused]] auto num = file->readInt();
        // read field name
        auto name = file->readString();
        // read field res
        if (auto cur = readType(file); cur->kind() != TypeKind::NOTYPE) {
            index = 0;
            type = cur;
        } else {
            index++;
        }
        // read field offset
        [[maybe_unused]] auto offset = file->readInt();
        fields.push_back(make_unique<FieldNode>(EMPTY_POS, make_unique<IdentDef>(name), type, index));
        // check for terminator
        ch = file->readChar();
    }
    const auto res = context_.getOrInsertRecordType(EMPTY_POS, EMPTY_POS, base_t, std::move(fields), module_);
    res->setSize(size);
    return res;
}

void SymbolImporter::setXRef(const unsigned ref, TypeNode *type) {
    xrefs_[ref] = type;
}

TypeNode *SymbolImporter::getXRef(unsigned ref) const {
    if (ref >= static_cast<int>(TypeKind::SET) && ref <= static_cast<int>(TypeKind::STRING)) {
        return system_.getBasicType(static_cast<TypeKind>(ref));
    }
    if (ref >= static_cast<int>(TypeKind::TYPE)) {
        if (const auto it = xrefs_.find(ref); it != xrefs_.end()) {
            return it->second;
        }
    }
    return nullptr;
}

ModuleNode *SymbolImporter::getOrCreateModule(const std::string &module) const {
    if (!symbols_->getModule(module)) {
        symbols_->addModule(module);
        context_.addExternalModule(make_unique<ModuleNode>(make_unique<Ident>(module)));
    }
    return context_.getExternalModule(module);
}
