(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE CaseTypeTest1;

TYPE
    P0 = POINTER TO R0;
    R0 = RECORD END;
    P1 = POINTER TO R1;
    R1 = RECORD (R0) a: INTEGER END;
    P2 = POINTER TO R2;
    R2 = RECORD (R1) b: INTEGER END;
    P3 = POINTER TO R3;
    R3 = RECORD (R2) c: INTEGER END;

VAR
    r0: R0; r1: R1; r2: R2; r3: R3;
    p0: P0; p1: P1; p2: P2; p3: P3;

PROCEDURE CheckRec(VAR r: R0): INTEGER;
VAR res: INTEGER;
BEGIN
    CASE r OF
        R3: res := 3 | R2: res := 2 | R1: res := 1 | R0: res := 0
    END;
    RETURN res
END CheckRec;

PROCEDURE CheckPtr(p: P0): INTEGER;
VAR res: INTEGER;
BEGIN
    CASE p OF
        P3: res := 3 | P2: res := 2 | P1: res := 1 | P0: res := 0
    END;
    RETURN res
END CheckPtr;

BEGIN
    ASSERT(CheckRec(r0) = 0);
    ASSERT(CheckRec(r1) = 1);
    ASSERT(CheckRec(r2) = 2);
    ASSERT(CheckRec(r3) = 3);
    NEW(p0); ASSERT(CheckPtr(p0) = 0); DISPOSE(p0);
    NEW(p1); ASSERT(CheckPtr(p1) = 1); DISPOSE(p1);
    NEW(p2); ASSERT(CheckPtr(p2) = 2); DISPOSE(p2);
    NEW(p3); ASSERT(CheckPtr(p3) = 3); DISPOSE(p3)
END CaseTypeTest1.