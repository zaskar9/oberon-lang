MODULE SymbolImport;
IMPORT SymbolExport, Oberon;

VAR t: Oberon.TimeSpec;

BEGIN
    SymbolExport.SetTime(t)
END SymbolImport.