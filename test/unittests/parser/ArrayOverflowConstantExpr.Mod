(*
  RUN: %oberon --run %s
  XFAIL: *
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