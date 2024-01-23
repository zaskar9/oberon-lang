(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Arithmetic7;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  c := -3 * 5;
  printf("%d\n", c);
  a := -3; b := 5;
  c := a * b;
  printf("%d\n", c)
END Test;

BEGIN
    Test
END Arithmetic7.
(*
    CHECK: -15
    CHECK: -15
*)