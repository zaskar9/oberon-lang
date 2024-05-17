(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  LEN("") = 2 Should be documented. Maybe unexpected result for some users
*)
MODULE BuiltinNewDispose;

IMPORT Out;

TYPE
  Date = RECORD day, month, year: INTEGER END;

PROCEDURE Test;
VAR
    d : POINTER TO Date;
BEGIN
    NEW(d);
    d.day := 26;
    d.month := 1;
    d.year := 2024;
    Out.Int(d.year, 0); Out.Ln;
    Out.Int(d.month, 0); Out.Ln;
    Out.Int(d.day, 0); Out.Ln;
    DISPOSE(d);
    Out.Int(ORD(d = NIL), 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinNewDispose.
(*
    CHECK: 2024
    CHECK: 1
    CHECK: 26
    CHECK: 1
    CHECK-EMPTY
*)