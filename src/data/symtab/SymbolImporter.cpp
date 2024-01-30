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
    auto include = boost::filesystem::path(module).replace_extension("smb");
    auto fp = path_ / include;
    if (!boost::filesystem::exists(fp)) {
        logger_->debug("Symbol file not found: '" + fp.string() + "'.");
        auto opt = flags_->findInclude(include);
        if (opt.has_value()) {
            fp = opt.value();
        } else {
            logger_->debug("Symbol file not found: '" + include.string() + "'.");
            return nullptr;
        }
    }
    logger_->debug("Symbol file found: '" + fp.string() + "'.");
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
#ifdef _DEBUG
    std::cout << std::endl;
#endif

    // create namespace for module
    symbols_ = symbols;
    symbols_->createNamespace(alias);
    module_ = std::make_unique<ModuleNode>(EMPTY_POS, std::make_unique<Ident>(module));
    module_->setAlias(alias);
    auto ch = file->readChar();
    while (ch != 0 && !file->eof()) {
        auto nodeType = (NodeType) ch;
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
    file->flush();
    file->close();

#ifdef _DEBUG
    auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
    printer->print(module_.get());
#endif

    return std::move(module_);
}

void SymbolImporter::readDeclaration(SymbolFile *file, NodeType nodeType) {
    auto name = file->readString();
    auto ident = std::make_unique<QualIdent>(module_->getIdentifier()->name(), name);
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
                    logger_->error(file->path(), "Cannot import constant " + name + ".");
            }
            if (expr) {
                auto decl = std::make_unique<ConstantDeclarationNode>(EMPTY_POS, std::move(ident), std::move(expr));
                symbols_->import(module_->getAlias(), name, decl.get());
                module_->addConstant(std::move(decl));
            }
        }
    } else if (nodeType == NodeType::type_declaration) {
        auto decl = std::make_unique<TypeDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module_->getAlias(), name, decl.get());
        module_->addTypeDeclaration(std::move(decl));
    } else if (nodeType == NodeType::variable) {
        // read in export number
        [[maybe_unused]] auto exno = file->readInt();
        auto decl = std::make_unique<VariableDeclarationNode>(EMPTY_POS, std::move(ident), type);
        symbols_->import(module_->getAlias(), name, decl.get());
        module_->addVariable(std::move(decl));
    } else if (nodeType == NodeType::procedure) {
        auto decl = std::make_unique<ProcedureNode>(EMPTY_POS, std::move(ident));
        decl->setType(type);
        decl->setExtern(true);
        symbols_->import(module_->getAlias(), name, decl.get());
        module_->addProcedure(std::move(decl));
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
        logger_->error(file->path(), "export of pointer type kind not yet supported.");
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
        auto param = std::make_unique<ParameterNode>(EMPTY_POS, std::make_unique<Ident>("_"), ptype, (var == 0));
        param->setLevel(SymbolTable::MODULE_LEVEL);
        params.push_back(std::move(param));
        // check for terminator
        ch = file->readChar();
    }
    return context_->getOrInsertProcedureNode(nullptr, std::move(params), return_t);
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
    std::vector<std::unique_ptr<FieldNode>> fields;
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
        fields.push_back(std::make_unique<FieldNode>(EMPTY_POS, std::make_unique<Ident>(name), type));
        // check for terminator
        ch = file->readChar();
    }
    auto res = context_->getOrInsertRecordType(nullptr, std::move(fields));
    res->setBaseType(base_t);
    res->setSize(size);
    return res;
}