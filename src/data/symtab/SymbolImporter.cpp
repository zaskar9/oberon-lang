//
// Created by Michael Grossniklaus on 3/19/22.
//

#include "SymbolImporter.h"

#include <filesystem>
#include <memory>
#include <string>

#include "SymbolFile.h"
#include "data/ast/ProcedureNode.h"
#include "data/ast/NodePrettyPrinter.h"

using std::make_unique;
using std::string;
using std::unique_ptr;

std::unique_ptr<ModuleNode> SymbolImporter::read(const string &module, SymbolTable *symbols) {
    return read(module, module, symbols);
}

unique_ptr<ModuleNode> SymbolImporter::read(const string &alias, const string &name, SymbolTable *symbols) {
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
    auto file = std::make_unique<SymbolFile>();
    file->open(fp.string(), std::ios::in);

    // read symbol file header
    [[maybe_unused]] auto key = file->readLong();
    [[maybe_unused]] auto ident = file->readString();
    auto version = file->readChar();
    if (version != SymbolFile::VERSION) {
        logger_.error(fp.string(), "Wrong symbol file version.");
        return nullptr;
    }
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    // create namespace for name
    symbols_ = symbols;
    symbols_->createNamespace(alias);
    auto module = std::make_unique<ModuleNode>(std::make_unique<Ident>(name));
    module->setAlias(alias);
    auto ch = file->readChar();
    while (ch != 0 && !file->eof()) {
        auto nodeType = (NodeType) ch;
        readDeclaration(file.get(), nodeType, module.get());
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
    auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
    printer->print(module.get());
#endif
    return module;
}

void SymbolImporter::readDeclaration(SymbolFile *file, NodeType nodeType, ModuleNode *module) {
    auto name = file->readString();
    // auto ident = std::make_unique<QualIdent>(module->getIdentifier()->name(), name);
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
                    expr = std::make_unique<StringLiteralNode>(EMPTY_POS, file->readString(), type);
                    break;
                case TypeKind::INTEGER:
                    expr = std::make_unique<IntegerLiteralNode>(EMPTY_POS, file->readInt(), type);
                    break;
                case TypeKind::LONGINT:
                    expr = std::make_unique<IntegerLiteralNode>(EMPTY_POS, file->readLong(), type);
                    break;
                case TypeKind::REAL:
                    expr = std::make_unique<RealLiteralNode>(EMPTY_POS, file->readFloat(), type);
                    break;
                case TypeKind::LONGREAL:
                    expr = std::make_unique<RealLiteralNode>(EMPTY_POS, file->readDouble(), type);
                    break;
                default:
                    logger_.error(file->path(), "Cannot import constant " + name + ".");
            }
            if (expr) {
                auto decl = std::make_unique<ConstantDeclarationNode>(EMPTY_POS, std::move(ident), std::move(expr));
                symbols_->import(module->getAlias(), name, decl.get());
                decl->setModule(module);
                module->constants().push_back(std::move(decl));
            }
        }
    } else if (nodeType == NodeType::type) {
        auto decl = std::make_unique<TypeDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module->getAlias(), name, decl.get());
        decl->setModule(module);
        module->types().push_back(std::move(decl));
    } else if (nodeType == NodeType::variable) {
        // read in export number
        [[maybe_unused]] auto exno = file->readInt();
        auto decl = std::make_unique<VariableDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module->getAlias(), name, decl.get());
        decl->setModule(module);
        module->variables().push_back(std::move(decl));
    } else if (nodeType == NodeType::procedure) {
        auto decl = std::make_unique<ProcedureNode>(std::move(ident), dynamic_cast<ProcedureTypeNode *>(type), true);
        symbols_->import(module->getAlias(), name, decl.get());
        decl->setModule(module);
        module->procedures().push_back(std::move(decl));
    }
}

TypeNode *SymbolImporter::readType(SymbolFile *file) {
    auto ref = file->readChar();
    if (ref < 0) {
        ref *= -1;
        if (ref == (char) TypeKind::NOTYPE) {
            return nullptr;
        }
        return symbols_->getRef(static_cast<size_t>((unsigned) ref));
    }
    TypeNode *type = nullptr;
    auto kind = (TypeKind) file->readChar();
    if (kind == TypeKind::ARRAY) {
        type = readArrayType(file);
    } else if (kind == TypeKind::POINTER) {
        // TODO import pointer type
        logger_.error(file->path(), "export of pointer type kind not yet supported.");
    } else if (kind == TypeKind::PROCEDURE) {
        type = readProcedureType(file);
    } else if (kind == TypeKind::RECORD) {
        type = readRecordType(file);
    }
    if (type) {
        symbols_->setRef(static_cast<size_t>((unsigned) ref), type);
    }
    return type;
}

TypeNode *SymbolImporter::readArrayType(SymbolFile *file) {
    // read member type
    TypeNode *member_t = readType(file);
    // read dimension
    auto dimension = (unsigned) file->readInt();
    auto res = context_->getOrInsertArrayType(nullptr, dimension, member_t);
    // read in size
    res->setSize((unsigned) file->readInt());
    return res;
}

TypeNode *SymbolImporter::readProcedureType(SymbolFile *file) {
    // read return type
    TypeNode *return_t = readType(file);
    // read parameters
    std::vector<std::unique_ptr<ParameterNode>> params;
    auto ch = file->readChar();
    while (ch != 0) {
        auto var = file->readChar();
        auto ptype = readType(file);
        auto param = make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>("_"), ptype, (var == 0));
        param->setLevel(SymbolTable::MODULE_LEVEL);
        params.push_back(std::move(param));
        // check for terminator
        ch = file->readChar();
    }
    return context_->getOrInsertProcedureType(nullptr, std::move(params), return_t);
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
    auto ch = file->readChar();
    while (ch != 0) {
        // read field number
        [[maybe_unused]] auto num = file->readInt();
        // read field name
        auto name = file->readString();
        // read field res
        auto type = readType(file);
        // read field offset
        [[maybe_unused]] auto offset = file->readInt();
        fields.push_back(make_unique<FieldNode>(EMPTY_POS, make_unique<IdentDef>(name), type));
        // check for terminator
        ch = file->readChar();
    }
    auto res = context_->getOrInsertRecordType(nullptr, std::move(fields));
    res->setBaseType(base_t);
    res->setSize(size);
    return res;
}