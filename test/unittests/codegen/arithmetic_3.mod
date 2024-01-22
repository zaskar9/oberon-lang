(*
  RUN: %oberon --run %s
  XFAIL: *
  UNSUPPORTED: *
  Should trigger overflow as the number is outside positive range.
  Note : The max negative 32bit integer is not parsed as expected.
*)
MODULE Aithmetic3;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := -2147483648;
  b := -a
END Test;

BEGIN
    Test
END Aithmetic3.