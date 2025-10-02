//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "SymbolExporter.h"

using std::filesystem::path;

void SymbolExporter::write(const std::string &name, SymbolTable *symbols) {
    xref_ = static_cast<int>(TypeKind::TYPE) + 1;
    path pth;
    auto symdir = config_.getSymDir();
    if (symdir.empty()) {
        pth = context_.getSourceFileName().parent_path();
    } else {
        pth = symdir;
    }
    const auto fp = (pth / name).replace_extension(".smb");
    const auto file = std::make_unique<SymbolFile>();
    file->open(fp.string(), std::ios::out);
    // write symbol file header
    file->writeLong(0); // placeholder, inserted at the end
    file->writeString(name + ".Mod");
    file->writeChar(SymbolFile::VERSION);
#ifdef _DEBUG
    std::cout << std::endl;
#endif
    // open the modules global scope
    auto scope = symbols->getModule(name);
    // navigate from global scope to module scope
    scope = scope->getChild();
    // get exported symbols from the module scope
    std::vector<DeclarationNode *> exports;
    scope->getExportedSymbols(exports);
    // write out exported symbols to file
    for (const auto decl: exports) {
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
    if (nodeType == NodeType::constant) {
        const auto kind = decl->getType()->kind();
        if (kind == TypeKind::PROCEDURE) {
            // TODO write out export number ("exno" in Wirth's code)
            file->writeChar(-1);
        } else {
            const auto con = dynamic_cast<ConstantDeclarationNode*>(decl);
            switch (kind) {
                case TypeKind::STRING:
                    file->writeString(dynamic_cast<StringLiteralNode *>(con->getValue())->value());
                    break;
                case TypeKind::BOOLEAN:
                    file->writeChar(dynamic_cast<BooleanLiteralNode *>(con->getValue())->value() ? 1 : 0);
                    break;
                case TypeKind::CHAR:
                    file->writeChar(static_cast<int8_t>(dynamic_cast<CharLiteralNode *>(con->getValue())->value()));
                    break;
                case TypeKind::SHORTINT:
                    file->writeShort(static_cast<int16_t>(dynamic_cast<IntegerLiteralNode *>(con->getValue())->value()));
                    break;
                case TypeKind::INTEGER:
                    file->writeInt(static_cast<int32_t>(dynamic_cast<IntegerLiteralNode *>(con->getValue())->value()));
                    break;
                case TypeKind::LONGINT:
                    file->writeLong(dynamic_cast<IntegerLiteralNode *>(con->getValue())->value());
                    break;
                case TypeKind::REAL:
                    file->writeFloat(static_cast<float>(dynamic_cast<RealLiteralNode *>(con->getValue())->value()));
                    break;
                case TypeKind::LONGREAL:
                    file->writeDouble(dynamic_cast<RealLiteralNode *>(con->getValue())->value());
                    break;
                case TypeKind::SET:
                    file->writeInt(static_cast<int>(dynamic_cast<SetLiteralNode *>(con->getValue())->value().to_ulong()));
                    break;
                default:
                    logger_.error(file->path(), "Cannot export constant " + to_string(*decl->getIdentifier()) + ".");
            }
        }
    } else if (nodeType == NodeType::variable) {
        // TODO write out export number ("exno" in Wirth's code)
        file->writeChar(-1);
    }
}

void SymbolExporter::writeType(SymbolFile *file, TypeNode *type) {
    // unknown type
    if (!type) {
        file->writeChar(-static_cast<char>(TypeKind::NOTYPE));
        return;
    }
    // non-virtual basic type or string
    if ((type->isBasic() && !type->isVirtual()) || type->isString()) {
        file->writeChar(-static_cast<char>(type->kind()));
        return;
    }
    // declaration already serialized to file
    if (xrefs_.contains(type)) {
        file->writeChar(-static_cast<uint8_t>(xrefs_[type]));
        return;
    }
    // actual type that needs exporting
    if (type->isAnonymous()) {
        // anonymous type
        file->writeChar(0);
    } else {
        if (fwds_.contains(type)) {
            // resolve forward reference
            const unsigned ref = fwds_[type];
            file->writeChar(static_cast<signed char>(ref));
            xrefs_[type] = ref;
        } else {
            // create new reference for the type to be used within this symbol file
            file->writeChar(static_cast<signed char>(xref_));
            xrefs_[type] = xref_;
            xref_++;
        }
        if (isExternal(type)) {
            const auto decl = type->getDeclaration();
            file->writeString(decl->getModule()->getIdentifier()->name());
            file->writeString(decl->getIdentifier()->name());
        } else {
            file->writeString("");
        }
    }
    file->writeChar(static_cast<signed char>(type->kind()));
    switch (type->kind()) {
        case TypeKind::ARRAY:
            writeArrayType(file, dynamic_cast<ArrayTypeNode*>(type));
            break;
        case TypeKind::POINTER:
            writePointerType(file, dynamic_cast<PointerTypeNode *>(type));
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
    // https://github.com/andreaspirklbauer/Oberon-module-imports/blob/master/Sources/FPGAOberon2013/ORB.Mod
}

void SymbolExporter::writeArrayType(SymbolFile *file, const ArrayTypeNode *type) {
    // write out member type
    writeType(file, type->types()[0]);
    // write out length
    file->writeInt(static_cast<int>(type->lengths()[0]));
    // write out size
    file->writeInt(static_cast<int>(type->getSize()));
}

void SymbolExporter::writePointerType(SymbolFile *file, const PointerTypeNode *type) {
    const auto base = type->getBase();
    // Note: call to isExternal() relies on short-circuiting based on the nullptr-check in isAnonymous().
    if (!base->isAnonymous() && !isExternal(base) && !xrefs_.contains(base)) {
        // create forward reference
        file->writeChar(-static_cast<int8_t>(xref_));
        fwds_[base] = xref_;
        xref_++;
    } else {
        writeType(file, base);
    }
}

void SymbolExporter::writeProcedureType(SymbolFile *file, ProcedureTypeNode *type) {
    writeType(file, type->getReturnType());
    for (auto &param : type->parameters()) {
        writeParameter(file, param.get());
    }
    file->writeChar(0);
}

void SymbolExporter::writeRecordType(SymbolFile *file, const RecordTypeNode *type) {
    // for extended records, write their base type (or TypeKind::NOTYPE) first
    writeType(file, type->getBaseType());
    // write out export number
    if (type->isAnonymous()) {
        file->writeChar(0);
    } else {
        // TODO write out export number ("exno" in Wirth's code)
        file->writeChar(-1);
    }
    // write out the number of fields in this record
    file->writeInt(static_cast<int>(type->getFieldCount()));
    // write out the size of the type, i.e., sum of the sizes of the type of all fields
    file->writeInt(static_cast<int>(type->getSize()));
    auto offset = 0u;
    for (size_t i = 0; i < type->getFieldCount(); i++) {
        const auto field = type->getField(i);
        // write out field node type
        file->writeChar(static_cast<signed char>(field->getNodeType()));
        // write out field number
        file->writeInt(static_cast<int>(i + 1));
        // write out field name
        if (field->getIdentifier()->isExported() ||
            (!type->isAnonymous() && type->getDeclaration()->getModule() != context_.getTranslationUnit())) {
            file->writeString(field->getIdentifier()->name());
        } else {
            // write a "hidden" field to preserve the correct record layout
            file->writeString("_");
        }
        // write out field type: if successive fields have the same type,
        // write it out the first time and write `NOTYPE` for following fields
        writeType(file, field->seqId() == 0 ? field->getType() : nullptr);
        // write out field offset
        file->writeInt(static_cast<int>(offset));
        offset += field->getType()->getSize();
    }
    // write out terminator
    file->writeChar(0);
}

void SymbolExporter::writeParameter(SymbolFile *file, const ParameterNode *param) {
    // write out numeric value for NodeType::parameter
    file->writeChar(static_cast<signed char>(param->getNodeType()));
    // write out whether this parameter is read-only (1) or a VAR-parameter (0)
    if (param->isVar()) {
        file->writeChar(0);
    } else {
        file->writeChar(1);
    }
    // write out parameter type: if successive parameters have the same type,
    // only write it out the first time and write `NOTYPE` for following parameters
    writeType(file, param->seqId() == 0 ? param->getType() : nullptr);
}

bool SymbolExporter::isExternal(const TypeNode *type) const {
    return type->getDeclaration()->getModule() != context_.getTranslationUnit();
}
