MODULE Nested;

CONST const0 = 10;

TYPE TPoint2D = RECORD x, y: INTEGER; color: RECORD r, g, b: INTEGER END END;

VAR var0: INTEGER;
    array0: ARRAY const0 OF INTEGER;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

(* Writes a string value to standard out. *)
PROCEDURE Write(str: STRING);
BEGIN
    printf(str)
END Write;

(* Writes a newline to standard out. *)
PROCEDURE WriteLn;
BEGIN
    printf("\n")
END WriteLn;

(* Writes an integer value to standard out. *)
PROCEDURE WriteInt(val: INTEGER);
BEGIN
    printf("%d", val)
END WriteInt;

PROCEDURE PrintArray;
VAR i: INTEGER;
BEGIN
    Write("[");
    i := 0;
    WHILE i < const0 DO
        WriteInt(array0[i]);
        IF i + 1 < const0 THEN Write(", ") END;
        i := i + 1
    END;
    Write("]");
    WriteLn
END PrintArray;

PROCEDURE Procedure1(param: INTEGER);
VAR var1: INTEGER;
    array1: ARRAY const0 OF TPoint2D;

    PROCEDURE Procedure2(param: INTEGER);
    VAR var2: INTEGER;

        PROCEDURE Procedure3(param: INTEGER);
        VAR var3: INTEGER;
        BEGIN
            var3 := const0;
            array0[const0 - 1] := param;
            array0[var3 - 2] := param;
            var0 := param;
            var1 := param;
            var2 := param;
            array1[var2 - var1 + 1].x := param;
            array1[var2 - var1 + 2].color.r := param
        END Procedure3;

    BEGIN
        var2 := 0;
        WriteInt(var2); WriteLn;
        Procedure3(param);
        WriteInt(var2); WriteLn
    END Procedure2;

BEGIN
    var0 := 0;
    var1 := 0;
    WriteInt(var0); WriteLn;
    WriteInt(var1); WriteLn;
    PrintArray();
    Procedure2(param);
    WriteInt(var0); WriteLn;
    WriteInt(var1); WriteLn;
    PrintArray
END Procedure1;

BEGIN
    Procedure1(100)
END Nested.