MODULE Conversion;
IMPORT Out;

TYPE
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD
        value: INTEGER;
        next: Node
    END;

VAR
    i: INTEGER;
    l: LONGINT;
    f: REAL;
    d: LONGREAL;
    n: Node;

PROCEDURE AssertIntEqual(i, val: INTEGER);
BEGIN
    ASSERT(i = val)
END AssertIntEqual;

PROCEDURE AssertLongIntEqual(i, val: LONGINT);
BEGIN
    ASSERT(i = val)
END AssertLongIntEqual;

PROCEDURE ReturnIntAsLongInt(): LONGINT;
VAR i: INTEGER;
BEGIN
    i := 0;
    RETURN i
END ReturnIntAsLongInt;

PROCEDURE AssertRealEqual(i, val: REAL);
BEGIN
    ASSERT(i = val)
END AssertRealEqual;

PROCEDURE ReturnIntAsReal(): REAL;
VAR i: INTEGER;
BEGIN
    i := 0;
    RETURN i
END ReturnIntAsReal;

PROCEDURE AssertLongRealEqual(i, val: LONGREAL);
BEGIN
    ASSERT(i = val)
END AssertLongRealEqual;

PROCEDURE ReturnIntAsLongReal(): LONGREAL;
VAR i: INTEGER;
BEGIN
    i := 0;
    RETURN i
END ReturnIntAsLongReal;

PROCEDURE ReturnRealAsLongReal(): LONGREAL;
VAR f: REAL;
BEGIN
    f := 0;
    RETURN f
END ReturnRealAsLongReal;

PROCEDURE ReturnLongIntAsLongReal(): LONGREAL;
VAR l: LONGINT;
BEGIN
    l := 0;
    RETURN l
END ReturnLongIntAsLongReal;

PROCEDURE AssertPointerEqual(i, val: Node);
BEGIN
    ASSERT(i = val)
END AssertPointerEqual;

PROCEDURE ReturnNilAsNode(): Node;
BEGIN
    RETURN NIL
END ReturnNilAsNode;

PROCEDURE VarParameter(VAR l: LONGINT);
BEGIN
    l := l + 1
END VarParameter;

BEGIN
    i := 0;
    AssertIntEqual(i, 0);
    Out.Int(i, 0); Out.Ln;
    Out.String("INTEGER assertions passed."); Out.Ln;
    l := 0;
    AssertLongIntEqual(i, 0);
    AssertLongIntEqual(l, 0);
    AssertLongIntEqual(l, i);
    Out.Long(i, 0); Out.Ln;
    Out.Long(l, 0); Out.Ln;
    Out.String("LONGINT assertions passed."); Out.Ln;
    f := 0;
    f := i;
    AssertRealEqual(f, 0);
    AssertRealEqual(f, i);
    Out.Real(i, 0); Out.Ln;
    Out.Real(f, 0); Out.Ln;
    Out.String("REAL assertions passed."); Out.Ln;
    d := 0;
    d := i;
    d := l;
    d := f;
    AssertLongRealEqual(f, 0);
    AssertLongRealEqual(d, 0);
    AssertLongRealEqual(d, i);
    AssertLongRealEqual(d, l);
    AssertLongRealEqual(d, f);
    d := i * l;
    AssertLongRealEqual(d, i * l);
    Out.String("LONGREAL assertions passed."); Out.Ln;
    n := NIL;
    AssertPointerEqual(n, NIL);
    n := ReturnNilAsNode();
    AssertPointerEqual(n, NIL);
    Out.String("POINTER assertions passed."); Out.Ln;
    VarParameter(l);
    Out.String("All assertions passed."); Out.Ln
END Conversion.