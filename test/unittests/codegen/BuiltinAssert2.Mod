(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE BuiltinAssert2;

PROCEDURE Test;
BEGIN ASSERT(FALSE)
END Test;

BEGIN
    Test
END BuiltinAssert2.