MODULE ShortCircuit;
IMPORT Out;

VAR i: INTEGER;

PROCEDURE checkOne(i: INTEGER): BOOLEAN;
BEGIN
    Out.String("1 ");
    RETURN i # 0
END checkOne;

PROCEDURE checkTwo(i: INTEGER): BOOLEAN;
BEGIN
    Out.String("2 ");
    RETURN i > 0
END checkTwo;

PROCEDURE checkThree(i: INTEGER): BOOLEAN;
CONST Num = 100;
BEGIN
    Out.String("3 ");
    RETURN (Num DIV i = Num)
END checkThree;

PROCEDURE checkRange(i: INTEGER);
BEGIN
    IF (i >= 0) & (i < 10) OR (i >= 90) & (i < 100) THEN
        Out.String("passed")
    ELSE
        Out.String("not passed")
    END;
    Out.Ln
END checkRange;

PROCEDURE check(i: INTEGER);
BEGIN
    Out.Int(i); Out.String(": ");
    IF checkOne(i) & checkTwo(i) & checkThree(i) THEN
        Out.String("-> passed")
    ELSE
        Out.String("-> not passed")
    END;
    Out.Ln;
    Out.Int(i); Out.String(": ");
    IF checkOne(i) OR checkTwo(i) THEN
        Out.String("-> passed")
    ELSE
        Out.String("-> not passed")
    END;
    Out.Ln;
    Out.Int(i); Out.String(": ");
    IF (checkOne(i) OR checkTwo(i)) & checkThree(i) THEN
        Out.String("-> passed")
    ELSE
        Out.String("-> not passed")
    END;
    Out.Ln
END check;

BEGIN
    check(0);
    Out.Ln;
    check(-1);
    Out.Ln;
    check(1)
END ShortCircuit.
