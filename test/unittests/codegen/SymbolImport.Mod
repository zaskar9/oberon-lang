(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon -c %S/SymbolExport.Mod
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE SymbolImport;
IMPORT SymbolExport, Files, M := Math, O := Out, S := SYSTEM;

VAR r: Files.Rider;
    n: SymbolExport.Node;
    x, y: SymbolExport.BinTreeNode;

BEGIN
    SymbolExport.SetRider(r);
    ASSERT(~r.eof);
    SymbolExport.Swap(x, y);
    O.Real(M.pi, 10); O.Ln;
    O.Long(S.SIZE(INTEGER), 0); O.Ln;
    NEW(n);
    IF n IS SymbolExport.Object THEN O.String("TRUE") ELSE O.String("FALSE") END; O.Ln;
    DISPOSE(n)
END SymbolImport.
(*
  CHECK:  3.142E+00
  CHECK: 4
  CHECK: TRUE
*)