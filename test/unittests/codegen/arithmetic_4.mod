(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Arithmetic4;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  c := 10 DIV 3;
  printf("%d\n", c);
  c := 10 MOD 3;
  printf("%d\n", c);
  a := 10; b := 3;
  c := a DIV b;
  printf("%d\n", c);
  c := a MOD b;
  printf("%d\n", c)
END Test;

BEGIN
    Test
END Arithmetic4.
(*
    CHECK: 3
    CHECK: 1
    CHECK: 3
    CHECK: 1
*)