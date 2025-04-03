(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE ArrayOverflowConstantExpr;

PROCEDURE Test;
VAR x : ARRAY 3 OF INTEGER;
BEGIN
  x[0] := 1;
  x[1] := 2;
  x[2] := 3;
  x[4] := 4
END Test;

BEGIN
    Test()
END ArrayOverflowConstantExpr.
(*
  CHECK: {{.*}}:12:5:{{.*}}error:{{.*}}value 4 out of bounds [0..2].
*)