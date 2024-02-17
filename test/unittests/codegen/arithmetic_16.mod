(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Arithmetic16;

IMPORT Out;

PROCEDURE Test;
VAR
  a, b, c : REAL;
BEGIN
  a := -7.5;
  b := 3.5;
  c := a * b;
  Out.Real(c); Out.Ln;
  a := 7.5;
  b := 0;
  c := a * b;
  Out.Real(c); Out.Ln;
  a := 7.5;
  b := 3.5;
  c := a * b;
  Out.Real(c); Out.Ln
END Test;

BEGIN
    Test
END Arithmetic16.
(*
    CHECK: -26.25
    CHECK: 0
    CHECK: 26.25
*)