(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Array3;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR
  a : ARRAY 3 OF BOOLEAN;
  b : BOOLEAN;
  i : INTEGER;
BEGIN
  b := TRUE;
  FOR i := 0 TO 2 DO
    a[i] := b;
    b := ~b
  END;
  FOR i := 0 TO 2 DO
    IF a[i] THEN printf("1 ") ELSE printf("0 ") END
  END
END Test;

BEGIN
    Test
END Array3.
(*
    CHECK: 1 0 1 
*)