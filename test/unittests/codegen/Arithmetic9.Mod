(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE Arithmetic9;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 1073741824; b := 2;
  c := a * b
END Test;

BEGIN
    Test
END Arithmetic9.
(*
  CHECK: {{.*}}code 8 (integer overflow)
*)