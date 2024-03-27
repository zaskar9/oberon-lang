(*
  RUN: %oberon --run %s
  XFAIL: *
  REQUURES revision
  Divisor in MOD and DIV cannot be negative (cf. O07.8.2.2: x = q * y + r  and 0 <= r < y)
*)
MODULE Arithmetic5;

PROCEDURE Test;
VAR c : INTEGER;
BEGIN
  c := 10 MOD -3
END Test;

BEGIN
    Test
END Arithmetic5.