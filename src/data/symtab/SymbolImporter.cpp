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
    auto path = context_->getSourceFileName().parent_path();
    auto fp = (path / name).replace_extension(".smb");
    auto include = fp.filename();
    if (!std::filesystem::exists(fp)) {
        logger_.debug("Symbol file not found: '" + fp.string() + "'.");
        auto opt = config_.findInclude(include);
        if (opt.has_value()) {
            fp = opt.value();
        } else {
            logger_.debug("Symbol file not found: '" + include.string() + "'.");
            return nullptr;
        }
    }
    logger_.debug("Symbol file found: '" + fp.string() + "'.");
    auto file = make_unique<SymbolFile>();
    file->open(fp.string(), std::ios::in);

    // read symbol file header
    [[maybe_unused]] auto key = file->readLong();
    [[maybe_unused]] auto ident = file->readString();
    auto version = file->readChar();
    if (version != SymbolFile::VERSION) {
        logger_.error(fp.string(), "Wrong symbol file version: expected " + to_string(SymbolFile::VERSION) +
                                   ", found " + to_string((int) version) + ".");
        return nullptr;
    }
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    // get or create module
    auto module = getOrCreateModule(name);
    auto ch = file->readChar();
    while (ch != 0 && !file->eof()) {
        auto nodeType = (NodeType) ch;
        readDeclaration(file.get(), nodeType, module);
#ifdef _DEBUG
        std::cout << std::endl;
#endif
        // check for terminator
        ch = file->readChar();
    }
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    file->flush();
    file->close();
#ifdef _DEBUG
    auto printer = make_unique<NodePrettyPrinter>(std::cout);
    printer->print(module);
#endif
    return module;
}

void SymbolImporter::readDeclaration(SymbolFile *file, NodeType nodeType, ModuleNode *module) {
    auto name = file->readString();
    auto ident = make_unique<IdentDef>(name);
    auto type = readType(file);
    if (nodeType == NodeType::constant) {
        auto kind = type->kind();
        if (kind == TypeKind::PROCEDURE) {
            // read in export number
            [[maybe_unused]] auto exno = file->readInt();
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
                auto decl = make_unique<ConstantDeclarationNode>(std::move(ident), std::move(expr));
                symbols_->import(module->getIdentifier()->name(), name, decl.get());
                decl->setModule(module);
                module->constants().push_back(std::move(decl));
            }
        }
    } else if (nodeType == NodeType::type && !symbols_->lookup(module->getIdentifier()->name(), name)) {
        auto decl = make_unique<TypeDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module->getIdentifier()->name(), name, decl.get());
        decl->setModule(module);
        module->types().push_back(std::move(decl));
    } else if (nodeType == NodeType::variable) {
        // read in export number
        [[maybe_unused]] auto exno = file->readInt();
        auto decl = make_unique<VariableDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module->getIdentifier()->name(), name, decl.get());
        decl->setModule(module);
        module->variables().push_back(std::move(decl));
    } else if (nodeType == NodeType::procedure) {
        auto decl = make_unique<ProcedureNode>(std::move(ident), dynamic_cast<ProcedureTypeNode *>(type));
        symbols_->import(module->getIdentifier()->name(), name, decl.get());
        decl->setModule(module);
        module->procedures().push_back(std::move(decl));
    }
}

TypeNode *SymbolImporter::readType(SymbolFile *file, PointerTypeNode *ptr) {
    TypeNode *type = nullptr;
    auto ref = file->readChar();
    // handle type references
    if (ref < 0) {
        ref *= -1;
        if (ref == (char) TypeKind::NOTYPE) {
            return nullptr;
        }
        // check whether the referenced type has already been imported
        type = symbols_->getRef(ref);
        if (type) {
            return type;
        }
        // referenced type has not yet been imported, create a forward reference
        if (ptr) {
            forwards_[ref] = ptr;
        }
        return nullptr;
    }
    // handle re-exported types
    TypeDeclarationNode *decl = nullptr;
    string module, name;
    if (ref > 0) {
        module = file->readString();
        if (!module.empty()) {
            // check whether re-exported type has already been imported
            name = file->readString();
            decl = dynamic_cast<TypeDeclarationNode *>(symbols_->lookup(module, name));
        }
    }
    // read, possibly redundant, type description
    auto kind = (TypeKind) file->readChar();
    if (kind == TypeKind::ARRAY) {
        type = readArrayType(file);
    } else if (kind == TypeKind::POINTER) {
        type = readPointerType(file);
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
            auto external = getOrCreateModule(module);
            auto node = make_unique<TypeDeclarationNode>(EMPTY_POS, make_unique<IdentDef>(name), type);
            symbols_->import(module, name, node.get());
            node->setModule(external);
            external->types().push_back(std::move(node));
        }
    } else {
        // abort no declaration found and no type imported
        return nullptr;
    }
    symbols_->setRef(ref, type);
    // check if the imported type resolves a forward reference
    if (forwards_.contains(ref)) {
        forwards_[ref]->setBase(type);
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
    auto length = (unsigned) file->readInt();
    lengths.push_back(length);
    // check for multi-dimensional array
    if (type->isArray()) {
        auto array = dynamic_cast<ArrayTypeNode *>(type);
        dim += array->dimensions();
        lengths.insert(lengths.end(), array->lengths().begin(), array->lengths().end());
        types.insert(types.end(), array->types().begin(), array->types().end());
    }
    auto res = context_->getOrInsertArrayType(EMPTY_POS, EMPTY_POS, dim, lengths, types);
    // read in size
    res->setSize((unsigned) file->readInt());
    return res;
}

TypeNode *SymbolImporter::readPointerType(SymbolFile *file) {
    // create a pointer type with null base type
    auto ptr = context_->getOrInsertPointerType(EMPTY_POS, EMPTY_POS, nullptr);
    // try to read the base type
    auto base = readType(file, ptr);
    // base type successfully imported
    if (base) {
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
    int index = 0;
    auto ch = file->readChar();
    while (ch != 0) {
        auto var = file->readChar();
        auto cur = readType(file);
        if (cur) {
            index = 0;
            type = cur;
        } else {
            index++;
        }
        auto param = make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>("_"), type, (var == 0), index);
        param->setLevel(SymbolTable::MODULE_LEVEL);
        params.push_back(std::move(param));
        // check for terminator
        ch = file->readChar();
    }
    return context_->getOrInsertProcedureType(EMPTY_POS, EMPTY_POS, std::move(params), false, ret);
}

TypeNode *SymbolImporter::readRecordType(SymbolFile *file) {
    // TODO add sanity checks based on the currently unused information
    // for extended records, read base type of record type (or TypeKind::NOTYPE)
    auto *base_t = dynamic_cast<RecordTypeNode*>(readType(file));
    // read export number
    [[maybe_unused]] auto exno = file->readInt();
    // read the number of fields in this record
    [[maybe_unused]] auto fld_cnt = file->readInt();
    // read the size of the type, i.e., sum of the sizes of the types of all fields
    auto size = (unsigned) file->readInt();
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
        auto cur = readType(file);
        if (cur) {
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
    auto res = context_->getOrInsertRecordType(EMPTY_POS, EMPTY_POS, std::move(fields));
    res->setBaseType(base_t);
    res->setSize(size);
    return res;
}

ModuleNode *SymbolImporter::getOrCreateModule(const std::string &module) {
    if (!symbols_->getModule(module)) {
        symbols_->addModule(module);
        context_->addExternalModule(make_unique<ModuleNode>(make_unique<Ident>(module)));
    }
    return context_->getExternalModule(module);
}