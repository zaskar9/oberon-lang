(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Arithmetic14;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR 
  r : LONGREAL;
  a, b, c : REAL;
BEGIN
  a := 7.5;
  b := 15.0;
  c := (a - b) / 2.0;
  r := c;
  printf("%.9g", r)
END Test;

BEGIN
    Test
END Arithmetic14.
(*
    CHECK: -3.75
*)