(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Const1;

IMPORT Out;

CONST
  A = 1;
  B = 2 + A;
  S = "'";
  STR = "Oberon";
  SSTR = S + STR + S;
  F = 12.3;

PROCEDURE Test;
BEGIN
    Out.Int(A, 0); Out.Ln;
    Out.Int(B, 0); Out.Ln;
    Out.String(S); Out.Ln;
    Out.String(STR); Out.Ln;
    Out.String(SSTR); Out.Ln;
    Out.Real(F, 0); Out.Ln
END Test;

BEGIN
    Test()
END Const1.
(*
    CHECK: 1
    CHECK: 3
    CHECK: '
    CHECK: Oberon
    CHECK: 'Oberon'
    CHECK: 1.23E+01
*)