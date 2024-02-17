(*
  RUN: %oberon --run %s
  XFAIL: *
  REQUIRES: revision
  Should trigger overflow as the number is outside positive range.
  Note : The max negative 32bit integer is not parsed as expected.
*)
MODULE Arithmetic3;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := -2147483648;
  b := -a
END Test;

BEGIN
    Test
END Arithmetic3.