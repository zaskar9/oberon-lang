(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s

  This fails as ODD uses the modulo operator which is also undefined for 0 (zero division error)
  It can be replaced by 'x AND 01H' and check that the result is 0.
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
    CHECK: 0
    CHECK: 1
    CHECK: 0
    CHECK: 1
    CHECK: 0
    CHECK-EMPTY
*)