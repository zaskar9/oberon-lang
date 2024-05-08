(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinRol1;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 0;
    Out.Hex(ROL(i, 8)); Out.Ln;
    i := 0F000000FH;
    Out.Hex(ROL(i, 4)); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinRol1.
(*
    CHECK: 00000000
    CHECK: 000000FF
    CHECK-EMPTY
*)