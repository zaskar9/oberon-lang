(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE BuiltinAssert2;

PROCEDURE Test;
BEGIN
    ASSERT(FALSE)
END Test;

BEGIN
    Test
END BuiltinAssert2.
(*
  CHECK: {{.*}}code 7 (assertion violated)
*)