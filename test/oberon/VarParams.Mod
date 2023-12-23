MODULE VarParams;
IMPORT Out;

TYPE List = ARRAY 2 OF INTEGER;
     Tuple = RECORD fst, snd: INTEGER END;

VAR a: List;
    t: Tuple;

PROCEDURE SwapInt(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b;
    b := t
END SwapInt;

PROCEDURE SwapList1(VAR a: List);
BEGIN
    SwapInt(a[0], a[1])
END SwapList1;

PROCEDURE SwapList2(VAR a: List);
BEGIN
    SwapList1(a)
END SwapList2;

PROCEDURE SwapList3(a: List);
BEGIN
    SwapList1(a)
END SwapList3;

PROCEDURE SwapTuple1(VAR t: Tuple);
BEGIN
    SwapInt(t.fst, t.snd)
END SwapTuple1;

PROCEDURE SwapTuple2(VAR t: Tuple);
BEGIN
    SwapTuple1(t)
END SwapTuple2;

PROCEDURE SwapTuple3(t: Tuple);
BEGIN
    SwapTuple1(t)
END SwapTuple3;

BEGIN
    a[0] := 42; a[1] := 24;
    SwapList1(a);
    ASSERT((a[0] = 24) & (a[1] = 42));
    SwapList2(a);
    ASSERT((a[0] = 42) & (a[1] = 24));
    SwapList3(a);
    ASSERT((a[0] = 42) & (a[1] = 24));

    t.fst := 42; t.snd := 24;
    SwapTuple1(t);
    ASSERT((t.fst = 24) & (t.snd = 42));
    SwapTuple2(t);
    ASSERT((t.fst = 42) & (t.snd = 24));
    SwapTuple3(t);
    ASSERT((t.fst = 42) & (t.snd = 24));

    Out.String("[OK] assertions passed!"); Out.Ln
END VarParams.
