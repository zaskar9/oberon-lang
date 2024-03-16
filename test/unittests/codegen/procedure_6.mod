(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Fails : Current array implementation will be updated
*)
MODULE Procedure6;

IMPORT Out;

VAR
  str : ARRAY 25 OF CHAR;

PROCEDURE Capacity(str : ARRAY OF CHAR): LONGINT;
BEGIN RETURN LEN(str)
END Capacity;

PROCEDURE CapacityVar(VAR str : ARRAY OF CHAR): LONGINT;
BEGIN RETURN LEN(str)
END CapacityVar;

PROCEDURE Test();
BEGIN
    Out.Long(LEN(str), 0); Out.Ln();
    Out.Long(Capacity(str), 0); Out.Ln();
    Out.Long(CapacityVar(str), 0); Out.Ln();
    str := "test";
    Out.Long(LEN(str), 0); Out.Ln();
    Out.Long(Capacity(str), 0); Out.Ln();
    Out.Long(CapacityVar(str), 0); Out.Ln()
END Test;

BEGIN
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