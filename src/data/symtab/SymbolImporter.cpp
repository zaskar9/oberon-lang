//
// Created by Michael Grossniklaus on 3/19/22.
//

#include "SymbolImporter.h"
#include "SymbolFile.h"
#include "data/ast/ProcedureNode.h"
#include "data/ast/NodePrettyPrinter.h"

std::unique_ptr<ModuleNode> SymbolImporter::read(const std::string &module, SymbolTable *symbols) {
    return read(module, module, symbols);
}

std::unique_ptr<ModuleNode> SymbolImporter::read(const std::string &alias, const std::string &module, SymbolTable *symbols) {
    auto fp = (path_ / module).replace_extension(".smb");
    if (!boost::filesystem::exists(fp)) {
        logger_->debug(fp.string(), "File not found.");
        return nullptr;
    }
    auto file = std::make_unique<SymbolFile>();
    file->open(fp.string(), std::ios::in);

    // read symbol file header
    [[maybe_unused]] auto key = file->readLong();
    auto name = file->readString();
    auto version = file->readChar();
    if (version != SymbolFile::VERSION) {
        logger_->error(fp.string(), "Wrong symbol file version.");
        return nullptr;
    }

    // create namespace for module
    symbols_ = symbols;
    symbols_->createNamespace(alias);
    module_ = std::make_unique<ModuleNode>(EMPTY_POS, std::make_unique<Identifier>(module));
    module_->setAlias(alias);
    auto ch = file->readChar();
    while (ch != 0 && !file->eof()) {
        auto nodeType = (NodeType) ch;
        readDeclaration(file.get(), nodeType);
        ch = file->readChar();
    }

    file->flush();
    file->close();

    // auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
    // printer->print(module_.get());

    return std::move(module_);
}

void SymbolImporter::readDeclaration(SymbolFile *file, NodeType nodeType) {
    auto name = file->readString();
    if (nodeType == NodeType::constant) {
        // TODO constant declaration
    } else if (nodeType == NodeType::type_declaration) {
        // TODO type declaration
    } else if (nodeType == NodeType::variable) {
        // TODO variable declaration
    } else if (nodeType == NodeType::procedure) {
        auto type = readType(file);
        auto ident = std::make_unique<Identifier>(module_->getIdentifier()->name(), name);
        auto proc = std::make_unique<ProcedureNode>(EMPTY_POS, std::move(ident));
        proc->setType(type);
        proc->setExtern(true);
        symbols_->import(module_->getAlias(), name, proc.get());
        module_->addProcedure(std::move(proc));
    }
}

TypeNode *SymbolImporter::readType(SymbolFile *file) {
    auto ref = file->readChar();
    if (ref < 0) {
        ref *= -1;
        if (ref == (char) TypeKind::NILTYPE) {
            return nullptr;
        }
        return symbols_->getRef((size_t) ref);
    }
    auto kind = (TypeKind) file->readChar();
    if (kind == TypeKind::ARRAY) {
        return readArrayType(file);
    } else if (kind == TypeKind::POINTER) {
        // TODO import pointer type
    } else if (kind == TypeKind::PROCEDURE) {
        return readProcedureType(file);
    } else if (kind == TypeKind::RECORD) {
        return readRecordType(file);
    }
    return nullptr;
}

TypeNode *SymbolImporter::readArrayType([[maybe_unused]] SymbolFile *file) {
    return nullptr;
}

TypeNode *SymbolImporter::readProcedureType(SymbolFile *file) {
    auto tmp = std::make_unique<ProcedureTypeNode>(EMPTY_POS);
    auto type = tmp.get();
    module_->registerType(std::move(tmp));
    auto res = readType(file);
    type->setReturnType(res);
    auto ch = file->readChar();
    while (ch != 0) {
        auto var = file->readChar();
        auto ptype = readType(file);
        auto param = std::make_unique<ParameterNode>(EMPTY_POS, std::make_unique<Identifier>("_"), ptype, (var == 0));
        param->setLevel(SymbolTable::MODULE_LEVEL);
        type->addParameter(std::move(param));
        ch = file->readChar();
    }
    return type;
}

TypeNode *SymbolImporter::readRecordType([[maybe_unused]] SymbolFile *file) {
    return nullptr;
}