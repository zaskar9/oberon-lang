(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE RecordExt2;
IMPORT Out;

TYPE R0 = RECORD END;
     R1 = RECORD (R0) v: INTEGER END;
     P0 = POINTER TO R0;
     P1 = POINTER TO R1;

VAR p0: P0;
    p1: P1;

PROCEDURE Test(p: P0);
BEGIN
    IF p IS P1 THEN Out.String("PASS") ELSE Out.String("FAIL") END;
    Out.Ln
END Test;

BEGIN
    NEW(p0);
    Test(p0);
    DISPOSE(p0);
    NEW(p1);
    Test(p1);
    DISPOSE(p1)
END RecordExt2.
(*
    CHECK: FAIL
    CHECK: PASS
*)