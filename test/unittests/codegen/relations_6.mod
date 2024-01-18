(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  Segmentation fault
*)
MODULE Relations6;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  IF 0AX = 0AX THEN
    IF 0FX > 0AX THEN
      IF 0AX < 0FX THEN
        printf("PASS");
        RETURN
      END
    END
  END;
  printf("FAIL")
END Test;

BEGIN
    Test
END Relations6.
(*
    CHECK: PASS
*)