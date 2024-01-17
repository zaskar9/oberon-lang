(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Array1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR
  i : INTEGER;
  a : ARRAY 3 OF INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i] := i + 1
  END;
  printf("%d %d %d", a[0], a[1], a[2])
END Test;

BEGIN
    Test
END Array1.
(*
    CHECK: 1 2 3
*)