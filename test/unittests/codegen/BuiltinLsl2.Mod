(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE BuiltinLsl2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
    l : LONGINT;
BEGIN
    i := 0FFH;
    Out.Hex(LSL(i, -4)); Out.Ln
END Test;

BEGIN
    Test
END BuiltinLsl2.
(*
  CHECK: {{.*}}code 10 (implicit sign conversion)
*)