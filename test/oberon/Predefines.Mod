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

BEGIN
    IncDec
END Predefines.