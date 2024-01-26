(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Arithmetic16;

PROCEDURE Test;
VAR
  r : LONGREAL;
  a, b, c : REAL;
BEGIN
  a := -7.5;
  b := 3.5;
  c := a * b;
  r := c;
  printf("%.9g\n", r);
  a := 7.5;
  b := 0;
  c := a * b;
  r := c;
  printf("%.9g\n", r);
  a := 7.5;
  b := 3.5;
  c := a * b;
  r := c;
  printf("%.9g\n", r)
END Test;

BEGIN
    Test
END Arithmetic16.
(*
    CHECK: -26.25
    CHECK: 0
    CHECK: 26.25
*)