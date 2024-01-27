MODULE NestedVar;
IMPORT Out;

VAR var0: INTEGER;

PROCEDURE Proc1(VAR param1: INTEGER);
VAR var1: INTEGER;

    PROCEDURE Proc2(VAR param2: INTEGER);
    VAR var2: INTEGER;

        PROCEDURE Proc3(VAR param3: INTEGER);
        VAR var3: INTEGER;
        BEGIN
            var1 := param1;
            var2 := param2;
            var3 := param3;
            param1 := var1 + var2 + var3;
            param2 := var1 + var2;
            param3 := var1
        END Proc3;

    BEGIN
        var1 := param1;
        var2 := param2;
        param1 := var1 + var2;
        param2 := var1;
        Proc3(var2)
    END Proc2;

BEGIN
    var1 := param1;
    param1 := var1;
    Proc2(var1)
END Proc1;

BEGIN
    var0 := 1;
    Proc1(var0);
    Out.Int(var0, 0); Out.Ln
END NestedVar.