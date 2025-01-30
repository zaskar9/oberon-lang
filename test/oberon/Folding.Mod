MODULE Folding;
IMPORT Out;

CONST Ten = 10;

VAR
    i: INTEGER;
    r: REAL;

BEGIN
    i := 1;
    Out.Int(1 + 2 + 3, 0); Out.Ln;
    Out.Int(0 + i + (Ten - Ten * 1), 0); Out.Ln;
    Out.Int(0 + i, 0); Out.Ln;
    Out.Int(0 - i, 0); Out.Ln;
    Out.Int(i - 0, 0); Out.Ln;
    Out.Int(0 + 0, 0); Out.Ln
END Folding.