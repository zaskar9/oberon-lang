(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  Print "NOPASS" which should not be reached
*)
MODULE Loop1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  LOOP
    RETURN
  END;
  printf("NOPASS")
END Test;

BEGIN
    Test;
    printf("PASS")
END Loop1.
(*
    CHECK: PASS
*)