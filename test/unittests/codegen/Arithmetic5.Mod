(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE Arithmetic5;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 10; b := -3;
  c := a MOD b
END Test;

BEGIN
    Test
END Arithmetic5.
(*
  CHECK: {{.*}}code 10 (implicit sign conversion)
*)