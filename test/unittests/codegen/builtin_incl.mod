(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinIncl;

IMPORT Out;

PROCEDURE Test;
VAR
    s : SET;
BEGIN
    s := {};
    Out.Set(s); Out.Ln;
    INCL(s, 0);
    Out.Set(s); Out.Ln;
    INCL(s, 31);
    Out.Set(s); Out.Ln
END Test;

BEGIN
    Test
END BuiltinIncl.
(*
    CHECK: { }
    CHECK: { 0 }
    CHECK: { 0 31 }
    CHECK-EMPTY
*)