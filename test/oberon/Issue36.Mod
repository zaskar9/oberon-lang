MODULE Issue36;

PROCEDURE Run1();
VAR
    str : ARRAY 25 OF CHAR;
    PROCEDURE Assert(b: BOOLEAN);
    BEGIN
        ASSERT(b)
    END Assert;
BEGIN
    str := "test1";
END Run1;

PROCEDURE Run2();
VAR
    str : ARRAY 25 OF CHAR;
    PROCEDURE Assert(b: BOOLEAN);
    BEGIN
        ASSERT(b)
    END Assert;
BEGIN
    str := "test2";
END Run2;

BEGIN
    Run1();
    Run2()
END Issue36.