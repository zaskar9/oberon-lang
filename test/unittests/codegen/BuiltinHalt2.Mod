(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE BuiltinHalt2;

PROCEDURE Test;
BEGIN
    HALT(1)
END Test;

BEGIN
    Test
END BuiltinHalt2.