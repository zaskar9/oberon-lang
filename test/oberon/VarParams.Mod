MODULE VarParams;
IMPORT Out;

CONST const = 5;

TYPE List = ARRAY 2 OF INTEGER;
     Tuple = RECORD fst, snd: INTEGER END;

VAR a: List;
    t: Tuple;
    i, j: INTEGER;

PROCEDURE SwapInt1(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b;
    b := t
END SwapInt1;

PROCEDURE SwapInt2(a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b;
    b := t
END SwapInt2;

PROCEDURE SwapInt3(a, b: INTEGER);
BEGIN
    SwapInt1(a, b)
END SwapInt3;

PROCEDURE SwapList1(VAR a: List);
BEGIN
    SwapInt1(a[0], a[1])
END SwapList1;

PROCEDURE SwapList2(VAR a: List);
BEGIN
    SwapList1(a)
END SwapList2;

PROCEDURE SwapTuple1(VAR t: Tuple);
BEGIN
    SwapInt1(t.fst, t.snd)
END SwapTuple1;

PROCEDURE SwapTuple2(VAR t: Tuple);
BEGIN
    SwapTuple1(t)
END SwapTuple2;

BEGIN
    a[0] := 42; a[1] := 24;
    SwapList1(a);
    ASSERT((a[0] = 24) & (a[1] = 42));
    SwapList2(a);
    ASSERT((a[0] = 42) & (a[1] = 24));

    t.fst := 42; t.snd := 24;
    SwapTuple1(t);
    ASSERT((t.fst = 24) & (t.snd = 42));
    SwapTuple2(t);
    ASSERT((t.fst = 42) & (t.snd = 24));

    i := 42; j := 24;
    SwapInt1(i, j);
    ASSERT((i = 24) & (j = 42));
    SwapInt2(i, j);
    ASSERT((i = 24) & (j = 42));
    SwapInt3(i, j);
    ASSERT((i = 24) & (j = 42));

    Out.String("[OK] assertions passed!"); Out.Ln
END VarParams.
