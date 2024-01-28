(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
  UNSUPPORTED: *
  boolean constants not supported
*)
MODULE ConstBoolean;

IMPORT Out;

CONST
  A = FALSE;
  B = TRUE;

PROCEDURE Test;
BEGIN
  Out.Int(ORD(A), 0); Out.Ln;
  Out.Int(ORD(B), 0); Out.Ln
END Test;

BEGIN
    Test()
END ConstBoolean.
(*
    CHECK: 0
    CHECK: 1
*)