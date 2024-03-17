(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE System1;

IMPORT SYSTEM, Out;

PROCEDURE Test;
VAR 
    adr1, adr2 : LONGINT;
    x, y, z : INTEGER;
    a, b, c : CHAR;
    d, e, f : BOOLEAN;
BEGIN
    (* INTEGER *)
    x := 3; y := -3;
    adr1 := SYSTEM.ADR(x);
    adr2 := SYSTEM.ADR(y);
    SYSTEM.GET(adr1, z);
    SYSTEM.PUT(adr2, z);
    Out.Int(y, 0); Out.Ln;
    (* CHAR *)
    a := "a"; b := "b";
    adr1 := SYSTEM.ADR(a);
    adr2 := SYSTEM.ADR(b);
    SYSTEM.GET(adr1, c);
    SYSTEM.PUT(adr2, c);
    Out.Char(b); Out.Ln;
    (* BOOLEAN *)
    d := FALSE; e := TRUE;
    adr1 := SYSTEM.ADR(d);
    adr2 := SYSTEM.ADR(e);
    SYSTEM.GET(adr1, f);
    SYSTEM.PUT(adr2, f);
    Out.Int(ORD(e), 0); Out.Ln
END Test;

BEGIN
    Test
END System1.
(*
    CHECK: 3
    CHECK: a
    CHECK: 0
*)