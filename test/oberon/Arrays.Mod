MODULE Arrays;
IMPORT Out;

TYPE List = ARRAY 10 OF INTEGER;

VAR l: ARRAY 10 OF INTEGER;

PROCEDURE Init(VAR l: List; val: INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO 9 DO
        l[i] := i * val
    END
END Init;

PROCEDURE Print(l: List);
VAR i: INTEGER;
BEGIN
    Out.String("List<"); Out.Long(LEN(l), 0); Out.String(">[");
    FOR i := 9 TO 0 BY -1 DO
        Out.Int(l[i], 0);
        IF i > 0 THEN Out.String(", ") ELSE Out.String("]") END
    END;
    Out.Ln
END Print;

PROCEDURE Run();
VAR l: List;
BEGIN
    Init(l, 10);
    Print(l)
END Run;

BEGIN
    Init(l, 5);
    Print(l);
    Run;
    Print(l)
END Arrays.