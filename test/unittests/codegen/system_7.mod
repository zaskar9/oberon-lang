(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE System7;

IMPORT SYSTEM, Out;

PROCEDURE Test;
VAR 
    a : LONGINT;
    b : INTEGER;
    c : SHORTINT;
BEGIN
    a := 0DEADBEEFH;
    Out.LongHex(a); Out.Ln;
    b := SYSTEM.VAL(INTEGER, a);
    Out.Hex(b); Out.Ln;
    c := SYSTEM.VAL(SHORTINT, a);
    Out.Hex(c); Out.Ln;
    c := 0BEEFH;
    b := SYSTEM.VAL(INTEGER, c);
    Out.Hex(b); Out.Ln;
    a := SYSTEM.VAL(LONGINT, c);
    Out.LongHex(a); Out.Ln
END Test;

BEGIN
    Test
END System7.
(*
    CHECK: 00000000DEADBEEF
    CHECK: DEADBEEF
    CHECK: 0000BEEF
    CHECK: 0000BEEF
    CHECK: 000000000000BEEF
*)
