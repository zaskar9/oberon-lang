(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
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
  Out.Real(c, 10); Out.Ln;
  a := 7.5;
  b := 0;
  c := a * b;
  Out.Real(c, 10); Out.Ln;
  a := 7.5;
  b := 3.5;
  c := a * b;
  Out.Real(c, 10); Out.Ln
END Test;

BEGIN
    Test
END Arithmetic16.
(*
    CHECK: -2.625E+01
    CHECK:          0
    CHECK:  2.625E+01
*)