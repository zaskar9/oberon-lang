(*
  RUN: %oberon --run %s
  XFAIL: *
  From the language report 2016:
  x = q*y + r  and 0 <= r < y
*)
MODULE Arithmetic6;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 10; b := -3;
  c := a DIV b;
END Test;

BEGIN
    Test
END Arithmetic6.