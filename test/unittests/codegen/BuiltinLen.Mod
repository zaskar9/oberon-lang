(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinLen;

IMPORT Out;

PROCEDURE Test;
VAR
    arr1 : ARRAY 1 OF CHAR;
    arr9 : ARRAY 9 OF CHAR;
    arr12 : ARRAY 1 OF ARRAY 2 OF CHAR;
BEGIN
    Out.Long(LEN(""), 0); Out.Ln;
    Out.Long(LEN(arr1), 0); Out.Ln;
    Out.Long(LEN(arr9), 0); Out.Ln;
    Out.Long(LEN(arr12), 0); Out.Ln;
    Out.Long(LEN(arr12, 0), 0); Out.Ln;
    Out.Long(LEN(arr12, 1), 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinLen.
(*
    CHECK: 0
    CHECK: 1
    CHECK: 9
    CHECK: 1
    CHECK: 1
    CHECK: 2
    CHECK-EMPTY
*)