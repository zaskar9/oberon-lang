(*
  RUN: %oberon --run %s
  XFAIL: *
  Divisor in MOD and DIV cannot be negative (cf. O07.8.2.2: x = q * y + r  and 0 <= r < y)
*)
MODULE Arithmetic5;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 10; b := -3;
  c := a MOD b
END Test;

BEGIN
    Test
END Arithmetic5.