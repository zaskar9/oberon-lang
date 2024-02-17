(*
  RUN: %oberon --run %s | filecheck %s
  fails due to superfluous semicolon, should give warning 
*)
MODULE FailOnMissingSemicolon;

PROCEDURE Test;
BEGIN; END Test;

BEGIN
    Test
END FailOnMissingSemicolon.
(*
    CHECK: PASS
*)