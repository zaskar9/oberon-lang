(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinShort1;

IMPORT Out;

PROCEDURE Test;
VAR
    s : SHORTINT;
    i : INTEGER;
    l : LONGINT;
    lr : LONGREAL;
    r : REAL;
BEGIN
    i := 123;
    Out.Int(i, 0); Out.Ln;
    s := SHORT(i);
    Out.Int(s, 0); Out.Ln;
    i := -123;
    Out.Int(i, 0); Out.Ln;
    s := SHORT(i);
    Out.Int(s, 0); Out.Ln;
    l := 321;
    Out.Long(l, 0); Out.Ln;
    i := SHORT(l);
    Out.Int(i, 0); Out.Ln;
    l := -321;
    Out.Long(l, 0); Out.Ln;
    i := SHORT(l);
    Out.Int(i, 0); Out.Ln;
    lr := 1.0;
    Out.LongReal(lr, 0); Out.Ln;
    r := SHORT(lr);
    Out.Real(r, 0); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinShort1.
(*
    CHECK: 123
    CHECK: 123
    CHECK: -123
    CHECK: -123
    CHECK: 321
    CHECK: 321
    CHECK: -321
    CHECK: -321
    CHECK: 1.0E+000
    CHECK: 1.00E+00
    CHECK-EMPTY
*)