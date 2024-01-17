(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  fails due to superfluous semicolon, should give warning 
*)
MODULE FailOnMissingSemicolon;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  printf("PASS");
END Test;

BEGIN
    Test
END FailOnMissingSemicolon.
(*
    CHECK: PASS
*)