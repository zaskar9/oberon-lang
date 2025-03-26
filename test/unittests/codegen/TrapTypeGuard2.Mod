(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE TrapTypeGuard2;

TYPE P0 = POINTER TO R0;
     R0 = RECORD END;
     P1 = POINTER TO R1;
     R1 = RECORD (R0) v: INTEGER END;

VAR p: P0;

PROCEDURE Test(p: P0);
BEGIN
    p(P1).v := 0
END Test;

BEGIN
    NEW(p);
    Test(p);
    DISPOSE(p)
END TrapTypeGuard2.