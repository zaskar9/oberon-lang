MODULE Arrays;
IMPORT Out, Random;

CONST Dim = 10;

TYPE
    Array = ARRAY Dim OF INTEGER;
    Matrix = ARRAY Dim, Dim OF INTEGER;
    Cube = ARRAY Dim OF ARRAY Dim OF ARRAY Dim OF INTEGER;

VAR l: Array;
    m: Matrix;
    c: Cube;
    i: INTEGER;

PROCEDURE Len(a: ARRAY OF INTEGER): LONGINT;
BEGIN
    RETURN LEN(a)
END Len;

PROCEDURE Len3D(c: ARRAY OF ARRAY OF ARRAY OF INTEGER): LONGINT;
BEGIN
    RETURN c[0, 0, 0]
END Len3D;

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
    LEN(l);
    Out.String("Array<"); Out.Long(Len(l), 0); Out.String(">[");
    FOR i := Dim - 1 TO 0 BY -1 DO
        Out.Int(l[i], 2);
        IF i > 0 THEN Out.String(", ") ELSE Out.String("]") END
    END;
    Out.Ln
END Print;

PROCEDURE InitMatrix(VAR m: Matrix; val: INTEGER);
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO Dim - 1 DO
        FOR j := 0 TO Dim - 1 DO
            m[i, j] := Random.Int(val)
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
    Random.Randomize;
    Init(l, 5);
    Print(l);
    Run;
    Print(l);
    Len(l);
    InitMatrix(m, 100);
    FOR i := 0 TO Dim - 1 DO
        Print(m[i])
    END;
    FOR i := 0 TO Dim - 1 DO
        c[i, i, i] := 42
    END;
    Out.Long(Len3D(c), 0); Out.Ln
END Arrays.