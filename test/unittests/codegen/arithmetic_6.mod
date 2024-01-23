(*
  RUN: %oberon --run %s
  XFAIL: *
  Negative operands should not be allowed?
*)
MODULE Arithmetic6;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 10; b := -3;
  c := a DIV b;
  printf("%d\n", c)
END Test;

BEGIN
    Test
END Arithmetic6.