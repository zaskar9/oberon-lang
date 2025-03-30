(* Example for Oberon07 Report, Chapter 9.5 *)
MODULE CaseTypeTest;

TYPE
    R = RECORD a: INTEGER END;
    R0 = RECORD (R) b: INTEGER END;
    R1 = RECORD (R) b: REAL END;
    R2 = RECORD (R) b: SET END;
    P = POINTER TO R;
    P0 = POINTER TO R0;
    P1 = POINTER TO R1;
    P2 = POINTER TO R2;

VAR p: P;

BEGIN
    CASE p OF
        P0: p.b := 10 |
        P1: p.b := 2.5 |
        P2: p.b := {0, 2}
    END
END CaseTypeTest.