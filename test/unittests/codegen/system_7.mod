(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE System7;

IMPORT SYSTEM, Out;

PROCEDURE Test;
VAR 
    x : LONGREAL;
    y : REAL;
    a : LONGINT;
    b : INTEGER;
    c : SHORTINT;
    d : SET;
BEGIN
    a := 1234567890;
    Out.Long(a, 0); Out.Ln;
    b := SYSTEM.VAL(INTEGER, a DIV 10);
    Out.Int(b, 0); Out.Ln;
    c := SYSTEM.VAL(SHORTINT, b DIV 10000);
    Out.Int(c, 0); Out.Ln;
    b := 0DEADBEEFH;
    d := SYSTEM.VAL(SET, b);
    Out.Hex(SYSTEM.VAL(INTEGER, d)); Out.Ln;
    x := -1.0;
    Out.LongHex(SYSTEM.VAL(LONGINT, x)); Out.Ln;
    y := -1.0;
    Out.Hex(SYSTEM.VAL(INTEGER, y)); Out.Ln;
    b := 0BF800000H;
    y := SYSTEM.VAL(REAL, b);
    Out.Real(y,0); Out.Ln
END Test;

BEGIN
    Test
END System7.
(*
    CHECK: 1234567890
    CHECK: 123456789
    CHECK: 12345
    CHECK: DEADBEEF
    CHECK: BFF0000000000000
    CHECK: BF800000
    CHECK:  -1.0E+00
*)