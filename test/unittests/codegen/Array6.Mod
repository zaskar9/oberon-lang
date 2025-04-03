(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE Array6;

PROCEDURE Test;
VAR
  i : INTEGER;
  a : ARRAY 3 OF INTEGER;
BEGIN
  FOR i := 0 TO 5 DO
    a[i] := i + 1
  END
END Test;

BEGIN
    Test
END Array6.
(*
  CHECK: {{.*}}code 1 (array index out of range)
*)