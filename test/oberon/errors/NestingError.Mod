MODULE NestingError;

PROCEDURE Top;

    PROCEDURE Nested;
    VAR i: INTEGER;
    BEGIN
        i := i + 1;

END Top;

BEGIN
    Top()
END NestingError.