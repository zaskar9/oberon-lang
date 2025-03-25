(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE ConstChar;

IMPORT Out;

CONST
  A = 0AX;
  B = 22X;

PROCEDURE Test;
BEGIN
  Out.Int(ORD(A), 0); Out.Ln;
  Out.Int(ORD(B), 0); Out.Ln
END Test;

BEGIN
    Test()
END ConstChar.
(*
    CHECK: 10
    CHECK: 34
*)