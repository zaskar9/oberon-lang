(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinAsr1;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 16;
    Out.Int(ASR(i, 1), 0); Out.Ln;
    i := -16;
    Out.Int(ASR(i, 1), 0); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinAsr1.
(*
    CHECK: 8
    CHECK: -8
    CHECK-EMPTY
*)