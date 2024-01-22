(*
  RUN: %oberon --run %s
  UNSUPPORTED: *
  Message : Terminator found in the middle of a basic block!
*)
MODULE BuiltinHalt1;

PROCEDURE Test;
BEGIN HALT(0)
END Test;

BEGIN
    Test
END MODULE BuiltinHalt1;
.