(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
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
(*
  CHECK: {{.*}}:18:1:{{.*}}error:{{.*}}unexpected token VAR.
*)