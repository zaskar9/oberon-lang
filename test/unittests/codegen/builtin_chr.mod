(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Note : Overflow not checked. This is in line with ProjectOberon behaviour.
*)
MODULE BuiltinChr;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 97; Out.Char(CHR(i)); Out.Ln;
    Out.Char(CHR(97)); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinChr.
(*
    CHECK: a
    CHECK: a
    CHECK-EMPTY
*)