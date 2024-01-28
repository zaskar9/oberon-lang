(*
  RUN: %oberon --run %s | filecheck %s
  fails due to superfluous semicolon, should give warning 
*)
MODULE FailOnMissingSemicolon;

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