(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Arithmetic8;

IMPORT Out;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  c := 3 * -5;
  Out.Int(c, 0);  Out.Ln;
  a := 3; b := -5;
  c := a * b;
  Out.Int(c, 0);  Out.Ln
END Test;

BEGIN
    Test
END Arithmetic8.
(*
    CHECK: -15
    CHECK: -15
*)