//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "SymbolExporter.h"

void SymbolExporter::write(const std::string &name, SymbolTable *symbols, const boost::filesystem::path& path) {
    // auto ref = ((int) TypeKind::RECORD) + 1;
    auto fp = change_extension(path, "smb");
    file_.open (fp.string(), std::ios::out | std::ios::binary);

    // placeholder for key that is inserted at the end
    writeInt(file_, 0);
    writeInt(file_, 0);
    writeString(file_, path.filename().string());
    writeChar(file_, VERSION_KEY);

    // open the modules global scope
    auto scope = symbols->openNamespace(name);
    // navigate from global scope to module scope
    scope = scope->getChild();
    // get exported symbols from the module scope
    std::vector<DeclarationNode *> exports;
    scope->getExportedSymbols(exports);

    for (auto e: exports) {
        std::cout << *e->getIdentifier() << std::endl;
    }
    file_.flush();
    file_.close();
}

void SymbolExporter::writeChar(std::ofstream &file, char val) {
    file.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

void SymbolExporter::writeInt(std::ofstream &file, int val) {
    file.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

void SymbolExporter::writeString(std::ofstream &file, const std::string &val) {
    file.write(reinterpret_cast<const char *>(&val), sizeof(val));
}