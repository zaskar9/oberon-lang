(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  ORD("") = 0 should probably be documented somewhere as it might be a surprise it compiles.
*)
MODULE BuiltinOrd;

IMPORT Out;

PROCEDURE Test;
VAR
    c : CHAR;
    s : SET;
    b : BOOLEAN;
BEGIN
    c := ""; Out.Int(ORD(c), 0); Out.Ln;
    Out.Int(ORD(""), 0); Out.Ln;
    c := "a"; Out.Int(ORD(c), 0); Out.Ln;
    Out.Int(ORD("a"), 0); Out.Ln;
    s := {}; Out.Int(ORD(s), 0); Out.Ln;
    Out.Int(ORD({}), 0); Out.Ln;
    s := {0}; Out.Int(ORD(s), 0); Out.Ln;
    Out.Int(ORD({0}), 0); Out.Ln;
    b := FALSE; Out.Int(ORD(b), 0); Out.Ln;
    Out.Int(ORD(FALSE), 0); Out.Ln;
    b := TRUE; Out.Int(ORD(b), 0); Out.Ln;
    Out.Int(ORD(TRUE), 0); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinOrd.
(*
    CHECK: 0
    CHECK: 0
    CHECK: 97
    CHECK: 97
    CHECK: 0
    CHECK: 0
    CHECK: 1
    CHECK: 1
    CHECK: 0
    CHECK: 0
    CHECK: 1
    CHECK: 1
    CHECK-EMPTY
*)