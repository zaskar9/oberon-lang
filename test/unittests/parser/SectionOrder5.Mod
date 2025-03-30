(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE SectionOrder5;

CONST
  A = 1;

TYPE
  INT = INTEGER;

PROCEDURE Test;
VAR i : INT;
BEGIN
  i := A
END Test;

VAR
  i : INT;

BEGIN
  i := A;
  Test
END SectionOrder5.