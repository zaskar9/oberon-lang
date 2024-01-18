(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Boolean1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  IF TRUE THEN
    IF TRUE = TRUE THEN
      IF ~FALSE THEN
        IF TRUE # FALSE THEN
          printf("PASS");
          RETURN
        END
      END
    END
  END;
  printf("FAIL")
END Test;

BEGIN
    Test
END Boolean1.
(*
    CHECK: PASS
*)