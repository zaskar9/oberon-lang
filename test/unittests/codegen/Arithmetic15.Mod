(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE Arithmetic15;

PROCEDURE Test;
VAR 
  a, b, c : REAL;
BEGIN
  a := -7.5;
  b := 0.0;
  c := a / b
END Test;

BEGIN
    Test
END Arithmetic15.
(*
  CHECK: {{.*}}code 9 (floating point division by zero)
*)