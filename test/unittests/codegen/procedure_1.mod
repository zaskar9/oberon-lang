(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Procedure1;

IMPORT Out;

PROCEDURE Test(a, b : INTEGER) : INTEGER;
BEGIN RETURN a + b
END Test;

BEGIN
    Out.Int(Test(1, 2), 0); Out.Ln
END Procedure1.
(*
    CHECK: 3
*)