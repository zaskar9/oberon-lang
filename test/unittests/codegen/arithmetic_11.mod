(*
  RUN: %oberon --run %s
  XFAIL: *
  REQUIRES: revision
  Should mixed arithmetics with integers be supported?
  Oberon-2 have the LONG and SHORT procedure to solve this explicit.
*)
MODULE Arithmetic11;

PROCEDURE Test;
VAR 
  i : INTEGER;
  l, r : LONGINT;
BEGIN
  i := 7;
  l := 15;
  r := l + i;
END Test;

BEGIN
    Test
END Arithmetic11.