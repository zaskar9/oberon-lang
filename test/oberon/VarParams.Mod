MODULE VarParams;
IMPORT Out;

TYPE List = ARRAY 2 OF INTEGER;

VAR a: List;

PROCEDURE Swap(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b;
    b := t
END Swap;

PROCEDURE one(VAR a: List);
BEGIN
    Swap(a[0], a[1])
END one;

PROCEDURE two(VAR a: List);
BEGIN
    one(a)
END two;

BEGIN
    a[0] := 42; a[1] := 24;
    one(a);
    Out.String("["); Out.Int(a[0]); Out.String(","); Out.Int(a[1]); Out.String("]"); Out.Ln;
    two(a);
    Out.String("["); Out.Int(a[0]); Out.String(","); Out.Int(a[1]); Out.String("]"); Out.Ln
END VarParams.