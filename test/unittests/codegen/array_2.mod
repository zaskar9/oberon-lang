(*
  RUN: %oberon --run %s | filecheck %s
  Fails due to REAL not promoted when passed as argument.
  Ref : https://en.cppreference.com/w/cpp/language/variadic_arguments
*)
MODULE Array2;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR
  i : INTEGER;
  a : ARRAY 3 OF REAL;
BEGIN
  FOR i := 0 TO 2 DO
    a[i] := i + 1.5
  END;
  printf("%.1f %.1f %.1f", a[0], a[1], a[2])
END Test;

BEGIN
    Test
END Array2.
(*
    CHECK: 1.5 2.5 3.5
*)