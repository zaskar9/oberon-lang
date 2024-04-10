MODULE Issue36;
IMPORT Out;

PROCEDURE Run1();
VAR
    str : ARRAY 25 OF CHAR;
    PROCEDURE Assert(b: BOOLEAN);
    BEGIN
        Out.String("Run1_Assert()"); Out.Ln;
        ASSERT(b)
    END Assert;
BEGIN
    str := "test1";
    Assert(TRUE)
END Run1;

PROCEDURE Run2();
VAR
    str : ARRAY 25 OF CHAR;
    PROCEDURE Assert(b: BOOLEAN);
    BEGIN
        Out.String("Run2_Assert()"); Out.Ln;
        ASSERT(b)
    END Assert;
BEGIN
    str := "test2";
    Assert(TRUE)
END Run2;

BEGIN
    Run1();
    Run2()
END Issue36.