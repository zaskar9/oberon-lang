(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinUnpk;

IMPORT Out;

PROCEDURE Test;
VAR
  r : REAL;
  lr : LONGREAL;
  i : INTEGER;
BEGIN
  r := 1.0;
  UNPK(r, i);
  Out.Real(r, 7); Out.Ln;
  Out.Int(i, 0); Out.Ln;
  r := 5.0E-2;
  UNPK(r, i);
  Out.Real(r, 7); Out.Ln;
  Out.Int(i, 0); Out.Ln;
  lr := 1.0;
  UNPK(lr, i);
  Out.LongReal(lr, 9); Out.Ln;
  Out.Int(i, 0); Out.Ln;
  lr := 5.0E-2;
  UNPK(lr, i);
  Out.LongReal(lr, 9); Out.Ln;
  Out.Int(i, 0); Out.Ln
END Test;

BEGIN
    Test()
END BuiltinUnpk.
(*
    CHECK: 1.00E+00
    CHECK: 0
    CHECK: 1.60E+00
    CHECK: -5
    CHECK: 1.0E+000
    CHECK: 0
    CHECK: 1.6E+000
    CHECK: -5
*)