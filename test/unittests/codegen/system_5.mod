(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE System5;

IMPORT SYSTEM, Out;

PROCEDURE Test;
VAR 
    adr : LONGINT;
    x : SET;
BEGIN
    x := {1, 31};
    adr := SYSTEM.ADR(x);
    Out.Int(ORD(SYSTEM.BIT(adr, 0)), 0); Out.Ln;
    Out.Int(ORD(SYSTEM.BIT(adr, 1)), 0); Out.Ln;
    Out.Int(ORD(SYSTEM.BIT(adr, 30)), 0); Out.Ln;
    Out.Int(ORD(SYSTEM.BIT(adr, 31)), 0); Out.Ln
END Test;

BEGIN
    Test
END System5.
(*
    CHECK: 0
    CHECK: 1
    CHECK: 0
    CHECK: 1
*)