(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE BuiltinAsr2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 16;
    Out.Int(ASR(i, -1), 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinAsr2.
(*
  CHECK: {{.*}}code 10 (implicit sign conversion)
*)