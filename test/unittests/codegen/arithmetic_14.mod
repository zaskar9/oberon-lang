(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Arithmetic14;

IMPORT Out;

PROCEDURE Test;
VAR 
  a, b, c : REAL;
BEGIN
  a := 7.5;
  b := 15.0;
  c := (a - b) / 2.0;
  Out.Real(c); Out.Ln
END Test;

BEGIN
    Test
END Arithmetic14.
(*
    CHECK: -3.75
*)