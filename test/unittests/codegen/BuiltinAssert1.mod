(*
  RUN: %oberon --run %s
*)
MODULE BuiltinAssert1;

PROCEDURE Test;
BEGIN ASSERT(TRUE)
END Test;

BEGIN
    Test
END BuiltinAssert1.