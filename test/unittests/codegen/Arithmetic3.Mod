(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE Arithmetic3;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := -2147483648;
  b := -a
END Test;

BEGIN
    Test
END Arithmetic3.
(*
  CHECK: {{.*}}code 8 (integer overflow)
*)