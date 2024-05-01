(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinLong1;

IMPORT Out;

PROCEDURE Test;
VAR
    s : SHORTINT;
    i : INTEGER;
    l : LONGINT;
    lr : LONGREAL;
    r : REAL;
BEGIN
    s := 123;
    Out.Int(s, 0); Out.Ln;
    i := LONG(s);
    Out.Int(i, 0); Out.Ln;
    s := -123;
    Out.Int(s, 0); Out.Ln;
    i := LONG(s);
    Out.Int(i, 0); Out.Ln;
    i := 321;
    Out.Int(i, 0); Out.Ln;
    l := LONG(i);
    Out.Long(l, 0); Out.Ln;
    i := -321;
    Out.Int(i, 0); Out.Ln;
    l := LONG(i);
    Out.Long(l, 0); Out.Ln;
    r := 1.0;
    Out.Real(r, 0); Out.Ln;
    lr := LONG(r);
    Out.LongReal(lr, 0); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinLong1.
(*
    CHECK: 123
    CHECK: 123
    CHECK: -123
    CHECK: -123
    CHECK: 321
    CHECK: 321
    CHECK: -321
    CHECK: -321
    CHECK: 1.00E+00
    CHECK: 1.0E+000
    CHECK-EMPTY
*)