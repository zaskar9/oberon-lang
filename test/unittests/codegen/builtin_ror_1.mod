(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinRor1;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 0;
    Out.Hex(ROR(i, 8)); Out.Ln;
    i := 0F000000FH;
    Out.Hex(ROR(i, 4)); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinRor1.
(*
    CHECK: 00000000
    CHECK: FF000000
    CHECK-EMPTY
*)