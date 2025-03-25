(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinMinMax;

IMPORT SYSTEM, Out;

PROCEDURE Test;
VAR 
    x : LONGREAL;
    y : REAL;
    a : LONGINT;
    b : INTEGER;
    c : SHORTINT;
BEGIN
    x := MAX(LONGREAL);
    Out.LongReal(x, 0); Out.Ln;
    x := MIN(LONGREAL);
    Out.LongReal(x, 0); Out.Ln;
    y := MAX(REAL);
    Out.Real(y, 0); Out.Ln;
    y := MIN(REAL);
    Out.Real(y, 0); Out.Ln;
    a := MAX(LONGINT);
    Out.Long(a, 0); Out.Ln;
    a := MIN(LONGINT);
    Out.Long(a, 0); Out.Ln;
    b := MAX(INTEGER);
    Out.Int(b, 0); Out.Ln;
    b := MIN(INTEGER);
    Out.Int(b, 0); Out.Ln;
    c := MAX(SHORTINT);
    Out.Int(c, 0); Out.Ln;
    c := MIN(SHORTINT);
    Out.Int(c, 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinMinMax.
(*
    CHECK: INF
    CHECK: -INF
    CHECK: INF
    CHECK: -INF
    CHECK: 9223372036854775807
    CHECK: -9223372036854775808
    CHECK: 2147483647
    CHECK: -2147483648
    CHECK: 32767
    CHECK: -32768
*)
