(*
  RUN: %oberon --run %s
  XFAIL: *
  Should trigger overflow as the number is outside positive range.
*)
MODULE Arithmetic3;
IMPORT Out;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := -2147483648;
  b := -a;
  Out.Int(b, 0); Out.Ln
END Test;

BEGIN
    Test
END Arithmetic3.