(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Array4;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR
  a : ARRAY 3 OF RECORD x : INTEGER END;
  i : INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i].x := i
  END;
  printf("%d %d %.d", a[0].x, a[1].x, a[2].x)
END Test;

BEGIN
    Test
END Array4.
(*
    CHECK: 0 1 2
*)