(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Relations1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  IF 2 = 2 THEN
    IF 2 > 1 THEN
      IF 1 < 2 THEN
        printf("PASS");
        RETURN
      END
    END
  END;
  printf("FAIL")
END Test;

BEGIN
    Test
END Relations1.
(*
    CHECK: PASS
*)