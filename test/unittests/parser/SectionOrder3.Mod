(*
  RUN: %oberon --run %s
*)
MODULE SectionOrder3;

CONST
  A = 1;

TYPE
  INT = INTEGER;

VAR
  i : INT;

BEGIN
  i := A
END SectionOrder3.