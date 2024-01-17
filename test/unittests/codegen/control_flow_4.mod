(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE ControlFlow4;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test : INTEGER;
VAR x : INTEGER;
BEGIN
  x := 3;
  IF x = 2 THEN RETURN 123
  ELSIF x = 3 THEN RETURN 213
  ELSE RETURN 321 END
END Test;

BEGIN
    printf("%d", Test())
END ControlFlow4.
(*
    CHECK: 213
*)