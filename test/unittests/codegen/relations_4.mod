(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Relations4;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := 1; b := 2;
  IF b # a THEN
    IF b > a THEN
      IF a < b THEN
        printf("PASS");
        RETURN
      END
    END
  END;
  printf("FAIL")
END Test;

BEGIN
    Test
END Relations4.
(*
    CHECK: PASS
*)