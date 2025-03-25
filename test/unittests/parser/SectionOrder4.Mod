(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE SectionOrder4;

TYPE
  INT = INTEGER;

CONST
  A = 1;

VAR
  i : INT;

BEGIN
  i := A
END SectionOrder4.