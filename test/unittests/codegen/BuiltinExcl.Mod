(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinExcl;

IMPORT Out;

PROCEDURE Test;
VAR
    s : SET;
BEGIN
    s := {0, 31};
    Out.Set(s); Out.Ln;
    EXCL(s, 0);
    Out.Set(s); Out.Ln;
    EXCL(s, 31);
    Out.Set(s); Out.Ln
END Test;

BEGIN
    Test
END BuiltinExcl.
(*
    CHECK: { 0 31 }
    CHECK: { 31 }
    CHECK: { }
    CHECK-EMPTY
*)