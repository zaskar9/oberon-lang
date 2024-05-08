(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinPack;

IMPORT Out;

PROCEDURE Test;
VAR
  r : REAL;
  lr : LONGREAL;
  i : INTEGER;
BEGIN
  r := 1.0;
  i := 0;
  r := PACK(r, i);
  Out.Real(r, 7); Out.Ln;
  r := 1.0;
  r := PACK(r, 0);
  Out.Real(r, 7); Out.Ln;
  r := 5.0;
  r := PACK(r, -2); (* 5.0*2.0^(-2) *)
  Out.Real(r, 7); Out.Ln;
  lr := 1.0;
  lr := PACK(lr, 0);
  Out.LongReal(lr, 10); Out.Ln;
  lr := PACK(5.0, -2);
  Out.LongReal(lr, 10); Out.Ln;
END Test;

BEGIN
    Test()
END BuiltinPack.
(*
    CHECK: 1.00E+00
    CHECK: 1.00E+00
    CHECK: 1.25E+00
    CHECK: 1.00E+000
    CHECK: 1.25E+000
*)