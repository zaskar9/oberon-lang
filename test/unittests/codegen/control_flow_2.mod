(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE ControlFlow2;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test : INTEGER;
BEGIN
  IF TRUE THEN RETURN 123
  ELSE RETURN 321 END
END Test;

BEGIN
    printf("%d", Test())
END ControlFlow2.
(*
    CHECK: 123
*)