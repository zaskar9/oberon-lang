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

BEGIN
    IncDec;
    NewFree
END Predefines.