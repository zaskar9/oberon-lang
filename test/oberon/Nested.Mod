MODULE Nested;

CONST Const1 = 27;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

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

PROCEDURE Procedure1(param2: INTEGER);
VAR var2: INTEGER;

    PROCEDURE Procedure2(param3: INTEGER);
    VAR var3: INTEGER;

        PROCEDURE Procedure3(param4: INTEGER);
        VAR var4: INTEGER;
        BEGIN
            var4 := Const1 + var2 + var3 + param4;
            WriteInt(var4); WriteLn
        END Procedure3;

    BEGIN
        var3 := Const1 + var2 + param3;
        Procedure3(var3);
        WriteInt(var3); WriteLn
    END Procedure2;

BEGIN
    var2 := Const1 + param2;
    Procedure2(var2);
    WriteInt(var2); WriteLn
END Procedure1;

BEGIN
    Procedure1(Const1)
END Nested.