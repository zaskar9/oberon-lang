(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Arithmetic13;

IMPORT Out;

PROCEDURE Test;
VAR 
  a, b, c : REAL;
BEGIN
  a := -7.5;
  b := 15.0;
  c := (a + b) / 2.0;
  Out.Real(c); Out.Ln
END Test;

BEGIN
    Test
END Arithmetic13.
(*
    CHECK: 3.75
*)