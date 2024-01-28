(*
  RUN: %oberon --run %s
  XFAIL: *
  REQUIRES: revision
  Should trigger overflow as the number is outside positive range of 32bit integer.
*)
MODULE Arithmetic9;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 1073741824; b := 2;
  c := a * b;
END Test;

BEGIN
    Test
END Arithmetic9.