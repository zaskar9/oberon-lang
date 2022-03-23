//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "SymbolExporter.h"

void SymbolExporter::write(const std::string &name, SymbolTable *symbols) {
    ref_ = ((int) TypeKind::STRING) + 1;
    auto fp = (path_ / name).replace_extension(".smb");
    auto file = std::make_unique<SymbolFile>();
    file->open(fp.string(), std::ios::out);

    // write symbol file header
    file->writeLong(0); // placeholder, inserted at the end
    file->writeString(name + ".Mod");
    file->writeChar(SymbolFile::VERSION);

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
    }
    file->writeChar(0);
    file->flush();
    file->close();
}

void SymbolExporter::writeDeclaration(SymbolFile *file, DeclarationNode *decl) {
    // write out node type
    file->writeChar((char) decl->getNodeType());
    // write out name
    file->writeString(decl->getIdentifier()->name());
    // write out type
    writeType(file, decl->getType());
}

void SymbolExporter::writeType(SymbolFile *file, TypeNode *type) {
    if (!type) {
        file->writeChar(-((char) TypeKind::NILTYPE));
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
    file->writeChar((char) type->kind());
    switch (type->kind()) {
        case TypeKind::ARRAY:
            writeArrayType(file, dynamic_cast<ArrayTypeNode*>(type));
            break;
        case TypeKind::POINTER:
            // TODO export pointer type
            logger_->error(type->pos(), "export of pointer type kind not yet supported.");
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
}

void SymbolExporter::writeArrayType(SymbolFile *file, ArrayTypeNode *type) {
    writeType(file, type->getMemberType());
    file->writeInt((int) type->getDimension());
    file->writeInt((int) type->getSize());
}

void SymbolExporter::writeProcedureType(SymbolFile *file, ProcedureTypeNode *type) {
    writeType(file, type->getReturnType());
    for (size_t i = 0; i < type->getParameterCount(); i++) {
        writeParameter(file, type->getParameter(i));
    }
    file->writeChar(0);
}

void SymbolExporter::writeRecordType(SymbolFile *file, RecordTypeNode *type) {
    // for extended records write their base type (or TypeKind::NOTYPE) first
    writeType(file, type->getBaseType());
    // write out export number
    if (type->isAnonymous()) {
        file->writeChar(0);
    } else {
        // TODO output "exno"
        // in Wirth's approach exports of record and procedure types are numbered
        file->writeChar(-1);
    }
    // write out the number of fields in this record
    file->writeInt(type->getFieldCount());
    // write out the size of the type, i.e., sum of the sizes of the type of all fields
    file->writeInt((int) type->getSize());
    auto offset = 0;
    for (size_t i = 0; i < type->getFieldCount(); i++) {
        auto field = type->getField(i);
        if (field->getIdentifier()->isExported()) {
            // write out field number
            file->writeInt(i);
            // write out field name
            file->writeString(field->getIdentifier()->name());
            // write out field type
            writeType(file, field->getType());
            // write out field offset
            file->writeInt(offset);
            offset += field->getType()->getSize();
        } else {
            // TODO find hidden pointers
        }
    }
}

void SymbolExporter::writeParameter(SymbolFile *file, ParameterNode *param) {
    // write out numeric value for NodeType::parameter
    file->writeChar((char) param->getNodeType());
    // write out whether this parameter is read-only (1) or a VAR-parameter (0)
    if (param->isVar()) {
        file->writeChar(0);
    } else {
        file->writeChar(1);
    }
    // write out parameter type
    writeType(file, param->getType());
}