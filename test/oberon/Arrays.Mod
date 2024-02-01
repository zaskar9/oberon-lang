MODULE Arrays;
IMPORT Out;

CONST Five = 5;

TYPE
    Array = ARRAY 10 * Five DIV 2 OF INTEGER;
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD
       val: INTEGER;
       next: Node
    END;
    List = RECORD
        first, last: Node
    END;

VAR l: Array;

PROCEDURE Init(VAR l: Array; val: INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO 9 DO
        l[i] := i * val
    END
END Init;

PROCEDURE Print(l: Array);
VAR i: INTEGER;
BEGIN
    Out.String("Array<"); Out.Long(LEN(l), 0); Out.String(">[");
    FOR i := 9 TO 0 BY -1 DO
        Out.Int(l[i], 0);
        IF i > 0 THEN Out.String(", ") ELSE Out.String("]") END
    END;
    Out.Ln
END Print;

PROCEDURE Run();
VAR l: Array;
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