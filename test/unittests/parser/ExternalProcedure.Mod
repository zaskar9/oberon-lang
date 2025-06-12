MODULE ExternalProcedure;
IMPORT Out;

VAR i: LONGINT;

    PROCEDURE [ "C" ] PrintF(format: ARRAY OF CHAR; ...); EXTERNAL [ "printf" ];

    PROCEDURE WriteLn(str: ARRAY OF CHAR);
    BEGIN
        PrintF("%s\n", str);
        Out.String(str); Out.Ln
    END WriteLn;

    PROCEDURE Count(): INTEGER;
    BEGIN
        RETURN 1
    END Count;

BEGIN
    WriteLn("Hello World!");
    Out.Int(Count(), 0); Out.Ln;
    i := SIZE(LONGINT);
    Out.Long(i, 0); Out.Ln
END ExternalProcedure.