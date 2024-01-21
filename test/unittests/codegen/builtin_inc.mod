(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE BuiltinInc;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR i : INTEGER;
BEGIN
  i := 0;
  INC(i);
  printf("%d ", i);
  INC(i, 10);
  printf("%d ", i);
  INC(i, -10);
  printf("%d ", i)
END Test;

BEGIN
    Test
END BuiltinInc.
(*
    CHECK: 1 11 1
*)