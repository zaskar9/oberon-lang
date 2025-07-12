(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE AliasedImport;
IMPORT AliasedImport := Out;
BEGIN
    AliasedImport.String("OK");
    AliasedImport.Ln;
END AliasedImport.
(*
  CHECK: OK
*)