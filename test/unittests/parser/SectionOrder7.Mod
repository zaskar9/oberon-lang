(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
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
(*
  CHECK: {{.*}}:9:1:{{.*}}error:{{.*}}unexpected token CONST.
*)