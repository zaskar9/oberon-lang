(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Procedure2;

IMPORT Out;

VAR c : INTEGER;

PROCEDURE Test(a, b : INTEGER; VAR c : INTEGER);
BEGIN c := a + b
END Test;

BEGIN
    Test(1, 2, c);
    Out.Int(c, 0); Out.Ln
END Procedure2.
(*
    CHECK: 3
*)