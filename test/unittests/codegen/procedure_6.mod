(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Procedure6;
IMPORT Out;

TYPE
    ShortString = ARRAY 25 OF CHAR;

VAR str: ShortString;

PROCEDURE Capacity(str: ARRAY OF CHAR): LONGINT;
BEGIN
    RETURN LEN(str)
END Capacity;

PROCEDURE CapacityVar(VAR str: ARRAY OF CHAR): LONGINT;
BEGIN
    RETURN LEN(str)
END CapacityVar;

PROCEDURE Test();
BEGIN
    Out.Long(LEN(str), 0); Out.Ln;
    Out.Long(Capacity(str), 0); Out.Ln;
    Out.Long(CapacityVar(str), 0); Out.Ln;
    str := "Test 1";
    Out.Long(LEN(str), 0); Out.Ln;
    Out.Long(Capacity(str), 0); Out.Ln;
    Out.Long(CapacityVar(str), 0); Out.Ln
END Test;

BEGIN
    str := "Test 0";
    Test
END Procedure6.
(*
    CHECK: 25
    CHECK: 25
    CHECK: 25
    CHECK: 25
    CHECK: 25
    CHECK: 25
*)