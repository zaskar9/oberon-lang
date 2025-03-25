(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE TrapTypeGuard1;

TYPE R0 = RECORD END;
     R1 = RECORD (R0) v: INTEGER END;

VAR r: R0;

PROCEDURE Test(VAR r: R0);
BEGIN
    r(R1).v := 0
END Test;

BEGIN
    Test(r)
END TrapTypeGuard1.