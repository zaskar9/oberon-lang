(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinOdd;

IMPORT Out;

PROCEDURE Test;
BEGIN
    Out.Int(ORD(ODD(1)), 0); Out.Ln;
    Out.Int(ORD(ODD(2)), 0); Out.Ln;
    Out.Int(ORD(ODD(0)), 0); Out.Ln;
    Out.Int(ORD(ODD(-2)), 0); Out.Ln;
    Out.Int(ORD(ODD(-1)), 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinOdd.
(*
    CHECK: 1
    CHECK: 0
    CHECK: 0
    CHECK: 0
    CHECK: 1
    CHECK-EMPTY
*)