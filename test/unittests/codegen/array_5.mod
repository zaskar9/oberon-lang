(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Array5;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE P;
BEGIN printf("1 ") END P;

PROCEDURE Test;
VAR
  a : ARRAY 3 OF RECORD p : PROCEDURE END;
  i : INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i].p := P
  END;
  FOR i := 0 TO 2 DO
    a[i].p()
  END
END Test;

BEGIN
    Test
END Array4.
(*
    CHECK: 1 1 1
*)