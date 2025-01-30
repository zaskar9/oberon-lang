MODULE TrapArrayBounds;

VAR a: ARRAY 3 OF INTEGER;
    i: INTEGER;

BEGIN
    FOR i := 0 TO 5 DO
        a[i] := i
    END
END TrapArrayBounds.