(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Arithmetic1;

IMPORT Out;

PROCEDURE Test;
VAR a, b, c, d : INTEGER;
BEGIN
  a := -1 ; b := 3 ; c := 7;
  d := a + b + c;
  Out.Int(d, 0); Out.Ln;
  d := c + (-b + a);
  Out.Int(d, 0); Out.Ln;
  d := (+a - b) + c;
  Out.Int(d, 0);  Out.Ln
END Test;

BEGIN
    Test
END Arithmetic1.
(*
    CHECK: 9
    CHECK: 3
    CHECK: 3
*)