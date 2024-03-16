(*
  RUN: %oberon --run %s
  XFAIL: *
  REQUIRES: revision
  Message : Terminator found in the middle of a basic block!
*)
MODULE BuiltinHalt2;

PROCEDURE Test;
BEGIN
    HALT(1)
END Test;

BEGIN
    Test
END BuiltinHalt2.