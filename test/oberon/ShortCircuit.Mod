MODULE ShortCircuit;
IMPORT Out;

VAR i: INTEGER;

PROCEDURE Trap(): BOOLEAN;
BEGIN
    ASSERT(FALSE);
    RETURN TRUE
END Trap;

PROCEDURE CheckTrap;
VAR i, j: INTEGER;
BEGIN
    i := 0;
    IF (i = 0) OR Trap() THEN
        ASSERT((i = 0) OR Trap())
    END;
    IF (i # 0) & Trap() THEN
        ASSERT((i # 0) & Trap())
    END;
    j := 1;
    IF (i = 0) & (j = 1) OR Trap() THEN
       ASSERT((i = 0) & (j = 1) OR Trap())
    END
END CheckTrap;

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
    RETURN (i > 0) & (Num DIV i = Num)
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
    Out.Int(i, 0); Out.String(": ");
    IF checkOne(i) & checkTwo(i) & checkThree(i) THEN
        Out.String("-> passed")
    ELSE
        Out.String("-> not passed")
    END;
    Out.Ln;
    Out.Int(i, 0); Out.String(": ");
    IF checkOne(i) OR checkTwo(i) THEN
        Out.String("-> passed")
    ELSE
        Out.String("-> not passed")
    END;
    Out.Ln;
    Out.Int(i, 0); Out.String(": ");
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
    check(1);
    CheckTrap;
    Out.String("All assertions passed."); Out.Ln
END ShortCircuit.
