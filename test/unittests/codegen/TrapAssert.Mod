(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE TrapAssert;

VAR b: BOOLEAN;

BEGIN
    b := FALSE;
    ASSERT(b)
END TrapAssert.