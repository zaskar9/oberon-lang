(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE BuiltinDec;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR i : INTEGER;
BEGIN
  i := 10;
  DEC(i);
  printf("%d ", i);
  DEC(i, 10);
  printf("%d ", i);
  DEC(i, -10);
  printf("%d ", i)
END Test;

BEGIN
    Test
END BuiltinDec.
(*
    CHECK: 9 -1 9
*)