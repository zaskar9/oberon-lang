(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinDec;

IMPORT Out;

PROCEDURE Test;
VAR i : INTEGER;
BEGIN
  i := 10;
  DEC(i);
  Out.Int(i, 0); Out.Ln;
  DEC(i, 10);
  Out.Int(i, 0); Out.Ln;
  DEC(i, -10);
  Out.Int(i, 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinDec.
(*
    CHECK: 9
    CHECK: -1
    CHECK: 9
*)