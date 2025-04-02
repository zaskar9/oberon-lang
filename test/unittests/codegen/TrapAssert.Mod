(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE TrapAssert;

VAR b: BOOLEAN;

BEGIN
    b := FALSE;
    ASSERT(b)
END TrapAssert.
(*
  CHECK: {{.*}}code 7 (assertion violated)
*)