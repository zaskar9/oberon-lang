MODULE Nested;
IMPORT Out;

CONST const0 = 10;

TYPE TPoint2D = RECORD x, y: INTEGER; color: RECORD r, g, b: INTEGER END END;

VAR var0: INTEGER;
    array0: ARRAY const0 OF INTEGER;

PROCEDURE PrintArray;
VAR i: INTEGER;
BEGIN
    Out.String("[");
    i := 0;
    WHILE i < const0 DO
        Out.Int(array0[i], 0);
        IF i + 1 < const0 THEN Out.String(", ") END;
        i := i + 1
    END;
    Out.String("]");
    Out.Ln
END PrintArray;

PROCEDURE Procedure1(param11: INTEGER; VAR param12: INTEGER);
VAR var1: INTEGER;
    array1: ARRAY const0 OF TPoint2D;

    PROCEDURE Procedure2(param2: INTEGER);
    VAR var2: INTEGER;
        array2: ARRAY const0 OF INTEGER;

        PROCEDURE Procedure3(param3: INTEGER);
        VAR var3: INTEGER;
        BEGIN
            var3 := const0;
            array0[const0 - 1] := param3;
            array0[var3 - 2] := param3;
            var0 := param3;
            var1 := param3;
            var2 := param3;
            array1[var2 - var1 + 1].x := param3;
            array1[var2 - var1 + 2].color.r := param3;
            ASSERT(LEN(array0) = const0);
            ASSERT(LEN(array1) = const0);
            ASSERT(LEN(array2) = const0);
            param12 := 0
        END Procedure3;

    BEGIN
        var2 := 0; ASSERT(var2 = 0);
        Procedure3(param2); ASSERT(var2 = 100)
    END Procedure2;

BEGIN
    var0 := 0; ASSERT(var0 = 0);
    var1 := 0; ASSERT(var1 = 0);
    param12 := 42; ASSERT(param12 = 42);
    PrintArray();
    Procedure2(param11); ASSERT(var0 = 100); ASSERT(var1 = 100);
    PrintArray
END Procedure1;

BEGIN
    Procedure1(100, var0);
    ASSERT(var0 = 0)
END Nested.