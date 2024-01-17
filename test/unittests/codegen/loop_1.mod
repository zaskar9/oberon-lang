(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  Goes into a infinite loop on EXIT statment
*)
MODULE Loop1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
  x := 2;
  LOOP
    IF x = 2 THEN
      IF x = 3 THEN
        RETURN
      END
    ELSIF x = 3 THEN
      RETURN
    ELSE
      RETURN
    END;
    EXIT
  END;
  printf("PASS")
END Test;

BEGIN
    Test
END Loop1.
(*
    CHECK: PASS
*)