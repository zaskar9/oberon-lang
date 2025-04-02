(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
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
(*
  CHECK: {{.*}}:9:1:{{.*}}error:{{.*}}unexpected token CONST.
*)