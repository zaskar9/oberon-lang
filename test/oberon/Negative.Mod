MODULE Negative;
IMPORT Out;

VAR i, j: INTEGER;

PROCEDURE Negate(i: INTEGER): INTEGER;
BEGIN
    RETURN -i
END Negate;

BEGIN
    i := 20; j := 10;
    i := 5 * -i + -j DIV 2;
    i := Negate(i);
    ASSERT(i = 105);
    ASSERT(5--5 = 10);
    ASSERT(5+-5 = 0);
    Out.String("Assertions passed."); Out.Ln
END Negative.