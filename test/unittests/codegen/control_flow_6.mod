(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE ControlFlow6;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
  x := 2;
  IF x = 2 THEN
    IF x = 3 THEN
      RETURN
    END
  ELSIF x = 3 THEN
    RETURN
  ELSE
    RETURN
  END;
  printf("PASS")
END Test;

BEGIN
    Test
END ControlFlow6.
(*
    CHECK: PASS
*)