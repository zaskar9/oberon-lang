//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "SymbolExporter.h"

void SymbolExporter::write([[maybe_unused]] const std::string &name, [[maybe_unused]] SymbolTable *symbols, boost::filesystem::path path) {
    auto fp = change_extension(path, "smb");
    file_.open (fp.string(), std::ios::out | std::ios::binary);
    char buffer[100];
    for (int i = 0; i < 100; i++) {
        buffer[i] = (char) i;
    }
    file_.write(buffer, 100);
    file_.flush();
    file_.close();
}