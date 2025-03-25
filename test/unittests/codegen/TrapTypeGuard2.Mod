(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE TrapTypeGuard2;

TYPE R0 = RECORD END;
     R1 = RECORD (R0) v: INTEGER END;
     P0 = POINTER TO R0;
     P1 = POINTER TO R1;

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