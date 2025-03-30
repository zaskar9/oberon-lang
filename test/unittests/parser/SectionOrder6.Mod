(*
  RUN: %oberon --run %s
*)
MODULE SectionOrder6;

CONST
  A = 1;

TYPE
  INT = INTEGER;

VAR
  i : INT;

PROCEDURE Test;
VAR i : INT;
BEGIN
  i := A
END Test;

BEGIN
  i := A;
  Test
END SectionOrder6.