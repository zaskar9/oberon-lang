(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  Illegal instruction
*)
MODULE Relations5;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  IF 2.5 = 2.5 THEN
    IF 2.5 > -1.5 THEN
      IF -1.5 < 2.5 THEN
        printf("PASS");
        RETURN
      END
    END
  END;
  printf("FAIL")
END Test;

BEGIN
    Test
END Relations5.
(*
    CHECK: PASS
*)