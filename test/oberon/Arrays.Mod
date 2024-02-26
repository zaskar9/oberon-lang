MODULE Arrays;
IMPORT Out;

CONST Dim = 10;

TYPE
    Array = ARRAY Dim OF INTEGER;
    Matrix = ARRAY Dim, Dim OF INTEGER;
    Cube = ARRAY Dim OF ARRAY Dim OF ARRAY Dim OF INTEGER;

VAR l: Array;
    m: Matrix;
    c: Cube;

PROCEDURE Len(a: ARRAY OF INTEGER): LONGINT;
BEGIN
    RETURN LEN(a)
END Len;

PROCEDURE Init(VAR l: Array; val: INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO Dim - 1 DO
        l[i] := i * val
    END
END Init;

PROCEDURE Print(l: Array);
VAR i: INTEGER;
BEGIN
    Out.String("Array<"); Out.Long(Len(l), 0); Out.String(">[");
    FOR i := Dim - 1 TO 0 BY -1 DO
        Out.Int(l[i], 0);
        IF i > 0 THEN Out.String(", ") ELSE Out.String("]") END
    END;
    Out.Ln
END Print;

PROCEDURE InitMatrix(VAR m: Matrix; val: INTEGER);
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO Dim - 1 DO
        FOR j := 0 TO Dim - 1 DO
            m[i, i] := val
        END
    END
END InitMatrix;

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
    Print(l);
    Len(l);
    c[0][0][0] := 42;
    Len(c)
END Arrays.