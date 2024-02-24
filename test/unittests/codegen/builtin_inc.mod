(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinInc;

IMPORT Out;

PROCEDURE Test;
VAR i : INTEGER;
BEGIN
  i := 0;
  INC(i);
  Out.Int(i, 0); Out.Ln;
  INC(i, 10);
  Out.Int(i, 0); Out.Ln;
  INC(i, -10);
  Out.Int(i, 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinInc.
(*
    CHECK: 1
    CHECK: 11
    CHECK: 1
*)