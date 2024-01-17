(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE ControlFlow3;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test : INTEGER;
VAR x : INTEGER;
BEGIN
  x := 2;
  IF x = 2 THEN RETURN 123
  ELSIF x = 3 THEN RETURN 213
  ELSE RETURN 321 END
END Test;

BEGIN
    printf("%d", Test())
END ControlFlow3.
(*
    CHECK: 123
*)