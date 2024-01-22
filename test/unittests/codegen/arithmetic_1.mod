(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Aithmetic1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR a, b, c, d : INTEGER;
BEGIN
  a := -1 ; b := 3 ; c := 7;
  d := a + b + c;
  printf("%d\n", d);
  d := c + (-b + a);
  printf("%d\n", d);
  d := (+a - b) + c;
  printf("%d\n", d)
END Test;

BEGIN
    Test
END Aithmetic1.
(*
    CHECK: 9
    CHECK: 3
    CHECK: 3
*)