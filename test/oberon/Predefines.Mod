MODULE Predefines;
IMPORT Out;

VAR a, b: INTEGER;

PROCEDURE IncDec;
VAR i: INTEGER;
    l: LONGINT;
BEGIN
    i := 0;
    l := 0;
    INC(i, 10);
    INC(l);
    Out.String("["); Out.Int(i); Out.String(","); Out.Long(l); Out.String("]"); Out.Ln;
    DEC(i, 5);
    DEC(l);
    Out.String("["); Out.Int(i); Out.String(","); Out.Long(l); Out.String("]"); Out.Ln
END IncDec;

PROCEDURE NewFree;
TYPE Person = POINTER TO PersonDesc;
     PersonDesc = RECORD
                      name: STRING;
                      title: ARRAY 3 OF STRING;
                      zipcode: INTEGER
                  END;
VAR p: Person;
BEGIN
    NEW(p);
    p.name := "Julien Zweifel";
    p.title[0] := "Mr.";
    p.zipcode := 8036;
    FREE(p)
END NewFree;

PROCEDURE LslAsr;
VAR x, n: LONGINT;
BEGIN
    x := 32;
    n := 2;
    Out.Long(LSL(x, n)); Out.String(",");
    Out.Long(ASR(x, n)); Out.Ln
END LslAsr;

PROCEDURE RolRor;
VAR x, n: LONGINT;
BEGIN
    x := -32;
    n := 2;
    Out.Long(ROL(x, n)); Out.String(",");
    Out.Long(ROR(x, n)); Out.Ln
END RolRor;

PROCEDURE Odd(x: INTEGER);
BEGIN
    Out.Int(x);
    IF ODD(x) THEN
        Out.String(" is odd.")
    ELSE
        Out.String(" is even.")
    END;
    Out.Ln
END Odd;

BEGIN
    a := 1;
    b := 0;
    NewFree;
    IncDec;
    LslAsr;
    RolRor;
    Odd(9);
    Odd(32);
    ASSERT(a > b);
    Out.String("Passed assertion."); Out.Ln;
    IF (a > b) THEN HALT(1) END;
    Out.String("Passed halt."); Out.Ln;
    HALT(0);
    Out.String("Unreachable!")
END Predefines.