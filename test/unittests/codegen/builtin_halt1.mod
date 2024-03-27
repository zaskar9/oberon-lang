(*
  RUN: %oberon --run %s
*)
MODULE BuiltinHalt1;

PROCEDURE Test;
BEGIN
    HALT(0)
END Test;

BEGIN
    Test
END BuiltinHalt1.