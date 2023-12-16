MODULE MissingBeginError;

VAR val: INTEGER;

PROCEDURE Inc(VAR val: INTEGER);
    val := val + 1
END Inc;

BEGIN
    val := 42;
    Inc(val)
END MissingBeginError.