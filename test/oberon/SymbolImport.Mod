MODULE SymbolImport;
IMPORT SymbolExport, Oberon, M := Math, O := Out, S := SYSTEM;

VAR t: Oberon.TimeSpec;

BEGIN
    t.secs := 0;
    t.nsecs := 0;
    SymbolExport.SetTime(t);
    O.Real(M.pi, 10); O.Ln;
    O.Long(S.SIZE(INTEGER), 0); O.Ln
END SymbolImport.