(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE Arithmetic10;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 1; b := 0;
  c := a DIV b
END Test;

BEGIN
    Test
END Arithmetic10.
(*
  CHECK: {{.*}}code 6 (integer division by zero)
*)