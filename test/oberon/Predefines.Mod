MODULE Predefines;
IMPORT Out;

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

PROCEDURE Test(VAR x: INTEGER);
BEGIN
    x := 10
END Test;

BEGIN
    NewFree;
    IncDec;
    LslAsr;
    RolRor
END Predefines.