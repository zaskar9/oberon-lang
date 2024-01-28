(*
  RUN: %oberon --run %s
  XFAIL: *
  From the language report 2016:
  x = q*y + r  and 0 <= r < y
*)
MODULE Arithmetic5;

PROCEDURE Test;
VAR c : INTEGER;
BEGIN
  c := 10 DIV -3;
END Test;

BEGIN
    Test
END Arithmetic5.