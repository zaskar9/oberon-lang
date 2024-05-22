(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE SectionOrder7;

PROCEDURE Test;
VAR
  i : INTEGER;
CONST
  I = 2;
BEGIN
    i := I
END Test;

BEGIN
    Test
END SectionOrder7.