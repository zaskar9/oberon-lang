(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Boolean2;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
  VAR t, f : BOOLEAN;
BEGIN
  t := TRUE;
  f := FALSE;
  IF t THEN
    IF t = TRUE THEN
      IF ~f THEN
        IF t # f THEN
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
END Boolean2.
(*
    CHECK: PASS
*)