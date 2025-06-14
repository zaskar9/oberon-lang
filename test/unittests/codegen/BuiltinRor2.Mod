(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE BuiltinRol2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 0F000000FH;
    Out.Hex(ROR(i, -4)); Out.Ln
END Test;

BEGIN
    Test
END BuiltinRol2.
(*
  CHECK: {{.*}}code 10 (implicit sign conversion)
*)