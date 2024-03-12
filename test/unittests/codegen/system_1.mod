(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE System1;

IMPORT SYSTEM, Out;

PROCEDURE Test;
VAR 
    xadr, yadr : LONGINT;
    x, y, z : INTEGER;
BEGIN
    x := 3; y := -3;
    xadr := SYSTEM.ADR(x);
    yadr := SYSTEM.ADR(y);
    SYSTEM.GET(xadr, z);
    SYSTEM.PUT(yadr, z);
    Out.Int(y, 0); Out.Ln
END Test;

BEGIN
    Test
END System1.
(*
    CHECK: 3
*)