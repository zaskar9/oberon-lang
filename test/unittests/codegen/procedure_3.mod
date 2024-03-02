(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Procedure3;

IMPORT Out;

VAR
  a, b, c : INTEGER;

PROCEDURE Test(a, b : INTEGER; VAR c : INTEGER);
BEGIN c := a + b
END Test;

BEGIN
    a := 1; b := 2;
    Test(a, b, c);
    Out.Int(c, 0); Out.Ln
END Procedure3.
(*
    CHECK: 3
*)