(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinAbs;

IMPORT Out;

PROCEDURE Test;
BEGIN
    Out.Int(ABS(0), 0); Out.Ln;
    Out.Int(ABS(-1000000), 0); Out.Ln;
    Out.Real(ABS(-0.0), 7); Out.Ln;
    Out.Real(ABS(-1000000.0), 7); Out.Ln
END Test;

BEGIN
    Test
END BuiltinAbs.
(*
    CHECK: 0
    CHECK: 1000000
    CHECK: 0
    CHECK: 1.00E+06
    CHECK-EMPTY
*)