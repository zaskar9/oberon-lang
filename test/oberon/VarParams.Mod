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

PROCEDURE three(a: List);
BEGIN
    one(a)
END three;

PROCEDURE print(a: List);
BEGIN
    Out.String("["); Out.Int(a[0]); Out.String(","); Out.Int(a[1]); Out.String("]"); Out.Ln
END print;

BEGIN
    a[0] := 42; a[1] := 24;
    print(a);
    one(a);
    print(a);
    two(a);
    print(a);
    three(a);
    print(a)
END VarParams.
