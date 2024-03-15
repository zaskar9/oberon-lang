//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "SymbolExporter.h"

using std::filesystem::path;

void SymbolExporter::write(const std::string &name, SymbolTable *symbols) {
    ref_ = ((int) TypeKind::STRING) + 1;
    path pth;
    auto symdir = config_.getSymDir();
    if (symdir == "") {
        pth = context_->getSourceFileName().parent_path();
    } else {
        pth = symdir;
    }
    auto fp = (pth / name).replace_extension(".smb");
    auto file = std::make_unique<SymbolFile>();
    file->open(fp.string(), std::ios::out);
    // write symbol file header
    file->writeLong(0); // placeholder, inserted at the end
    file->writeString(name + ".Mod");
    file->writeChar(SymbolFile::VERSION);
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    // open the modules global scope
    auto scope = symbols->getNamespace(name);
    // navigate from global scope to module scope
    scope = scope->getChild();
    // get exported symbols from the module scope
    std::vector<DeclarationNode *> exports;
    scope->getExportedSymbols(exports);
    // write out exported symbols to file
    for (auto decl: exports) {
        writeDeclaration(file.get(), decl);
#ifdef _DEBUG
        std::cout << std::endl;
#endif
    }
    // write out terminator
    file->writeChar(0);
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    // flush and close file
    file->flush();
    file->close();
}

void SymbolExporter::writeDeclaration(SymbolFile *file, DeclarationNode *decl) {
    // write out declaration node type
    auto nodeType = decl->getNodeType();
    file->writeChar(static_cast<signed char>(nodeType));
    // write out name
    file->writeString(decl->getIdentifier()->name());
    // write out type
    writeType(file, decl->getType());
    if (nodeType == NodeType::type) {
        // TODO check if this is the base type of a previously declared pointer type
    } else if (nodeType == NodeType::constant) {
        auto kind = decl->getType()->kind();
        if (kind == TypeKind::PROCEDURE) {
            // TODO write out export number ("exno" in Wirth's code)
            file->writeInt(-1);
        } else {
            auto con = dynamic_cast<ConstantDeclarationNode*>(decl);
            switch (kind) {
                case TypeKind::STRING:
                    file->writeString(dynamic_cast<StringLiteralNode*>(con->getValue())->value());
                    break;
                case TypeKind::INTEGER:
                    file->writeInt(dynamic_cast<IntegerLiteralNode*>(con->getValue())->value());
                    break;
                case TypeKind::LONGINT:
                    file->writeLong(dynamic_cast<IntegerLiteralNode*>(con->getValue())->value());
                    break;
                case TypeKind::REAL:
                    file->writeFloat(dynamic_cast<RealLiteralNode*>(con->getValue())->value());
                    break;
                case TypeKind::LONGREAL:
                    file->writeDouble(dynamic_cast<RealLiteralNode*>(con->getValue())->value());
                    break;
                default:
                    logger_.error(file->path(), "Cannot export constant " + to_string(*decl->getIdentifier()) + ".");
            }
        }
    } else if (nodeType == NodeType::variable) {
        // TODO write out export number ("exno" in Wirth's code)
        file->writeInt(-1);
    }
}

void SymbolExporter::writeType(SymbolFile *file, TypeNode *type) {
    if (!type) {
        file->writeChar(-((char) TypeKind::NOTYPE));
        return;
    }
    if (type->getRef() > 0) {
        // declaration already serialized to file
        file->writeChar(-type->getRef());
        return;
    }
    if (type->isAnonymous()) {
        // anonymous type
        file->writeChar(0);
    } else {
        // create new reference for the type to be used within this symbol file
        file->writeChar(ref_);
        type->setRef(ref_);
        ref_++;
    }
    file->writeChar(static_cast<signed char>(type->kind()));
    switch (type->kind()) {
        case TypeKind::ARRAY:
            writeArrayType(file, dynamic_cast<ArrayTypeNode*>(type));
            break;
        case TypeKind::POINTER:
            // TODO export pointer type
            logger_.error(type->pos(), "export of pointer type kind not yet supported.");
            break;
        case TypeKind::PROCEDURE:
            writeProcedureType(file, dynamic_cast<ProcedureTypeNode*>(type));
            break;
        case TypeKind::RECORD:
            writeRecordType(file, dynamic_cast<RecordTypeNode*>(type));
            break;
        default:
            // noting to be done
            break;
    }
    // TODO re-exports
}

void SymbolExporter::writeArrayType(SymbolFile *file, ArrayTypeNode *type) {
    // write out member type
    writeType(file, type->getMemberType());
    // write out length
    // TODO support for multi-dimensional arrays
    file->writeInt((int) type->lengths()[0]);
    // write out size
    file->writeInt((int) type->getSize());
}

void SymbolExporter::writeProcedureType(SymbolFile *file, ProcedureTypeNode *type) {
    writeType(file, type->getReturnType());
    for (auto &param : type->parameters()) {
        writeParameter(file, param.get());
    }
    file->writeChar(0);
}

void SymbolExporter::writeRecordType(SymbolFile *file, RecordTypeNode *type) {
    // for extended records, write their base type (or TypeKind::NOTYPE) first
    writeType(file, type->getBaseType());
    // write out export number
    if (type->isAnonymous()) {
        file->writeChar(0);
    } else {
        // TODO write out export number ("exno" in Wirth's code)
        file->writeInt(-1);
    }
    // write out the number of fields in this record
    file->writeInt(type->getFieldCount());
    // write out the size of the type, i.e., sum of the sizes of the type of all fields
    file->writeInt(static_cast<int>(type->getSize()));
    auto offset = 0u;
    for (size_t i = 0; i < type->getFieldCount(); i++) {
        auto field = type->getField(i);
        if (field->getIdentifier()->isExported()) {
            // write out field node type
            file->writeChar(static_cast<signed char>(field->getNodeType()));
            // write out field number
            file->writeInt(i + 1);
            // write out field name
            file->writeString(field->getIdentifier()->name());
            // write out field type
            writeType(file, field->getType());
            // write out field offset
            file->writeInt((int) offset);
            offset += field->getType()->getSize();
        } else {
            // TODO find hidden pointers
        }
    }
    // write out terminator
    file->writeChar(0);
}

void SymbolExporter::writeParameter(SymbolFile *file, ParameterNode *param) {
    // write out numeric value for NodeType::parameter
    file->writeChar(static_cast<signed char>(param->getNodeType()));
    // write out whether this parameter is read-only (1) or a VAR-parameter (0)
    if (param->isVar()) {
        file->writeChar(0);
    } else {
        file->writeChar(1);
    }
    // write out parameter type
    writeType(file, param->getType());
}