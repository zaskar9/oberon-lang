(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE ControlFlow1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test : INTEGER;
BEGIN
  IF TRUE THEN RETURN 123 END;
  RETURN 321
END Test;

BEGIN
    printf("%d", Test())
END ControlFlow1.
(*
    CHECK: 123
*)