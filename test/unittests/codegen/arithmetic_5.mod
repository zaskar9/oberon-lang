(*
  RUN: %oberon --run %s
  XFAIL: *
  UNSUPPORTED: *
  Expression is not parsed correctly?
*)
MODULE Arithmetic5;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR c : INTEGER;
BEGIN
  c := 10 DIV -3;
  printf("%d\n", c)
END Test;

BEGIN
    Test
END Arithmetic5.