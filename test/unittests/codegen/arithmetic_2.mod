(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Arithmetic2;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := 2147483646;
  b := a + 1;
  printf("%d\n", b);
  a := -2147483647;
  b := a - 1;
  printf("%d\n", b)
END Test;

BEGIN
    Test
END Arithmetic2.
(*
    CHECK: 214748364
    CHECK: -2147483648
*)