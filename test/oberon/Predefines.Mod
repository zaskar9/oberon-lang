MODULE Predefines;
IMPORT Out;

TYPE String = ARRAY 255 OF CHAR;

VAR a, b: INTEGER;

PROCEDURE IncDec;
VAR i: INTEGER;
    l: LONGINT;
    a: ARRAY 10 OF INTEGER;
    r: RECORD x: INTEGER END;
    p: POINTER TO RECORD x: INTEGER END;
BEGIN
    i := 0;
    l := 0;
    INC(i, 5);
    INC(l);
    Out.String("["); Out.Int(i, 0); Out.String(","); Out.Long(l, 0); Out.String("]"); Out.Ln;
    DEC(i, 5);
    DEC(l);
    Out.String("["); Out.Int(i, 0); Out.String(","); Out.Long(l, 0); Out.String("]"); Out.Ln;
    INC(i, i);
    INC(i, l); (* should trigger a warning *)
    INC(l, i);
    INC(l, l);
    INC(a[0]);
    INC(r.x);
    NEW(p); p.x := 0; INC(p.x); FREE(p)
END IncDec;

PROCEDURE NewFree;
TYPE
    Person = POINTER TO PersonDesc;
    PersonDesc = RECORD
        name: String;
        title: ARRAY 3 OF String;
        zipcode: INTEGER
    END;
VAR p: Person;
BEGIN
    NEW(p);
    p.name := "Julien Zweifel";
    p.title[0] := "Mr.";
    p.zipcode := 8036;
    FREE(p);
    ASSERT(p = NIL)
END NewFree;

PROCEDURE LslAsr;
VAR x, n: LONGINT;
BEGIN
    x := 32;
    n := 2;
    Out.Long(LSL(x, n), 0); Out.String(",");
    Out.Long(ASR(x, n), 0); Out.Ln
END LslAsr;

PROCEDURE RolRor;
VAR l, n: LONGINT;
    i: INTEGER;
BEGIN
    l := MIN(LONGINT);
    n := 63;
    Out.Long(ROL(l, n), 0); Out.String(",");
    Out.Long(ROR(l, n), 0); Out.Ln;
    i := MIN(INTEGER);
    n := 31;
    Out.Int(ROR(i, n), 0); Out.Ln
END RolRor;

PROCEDURE Odd(x: INTEGER);
BEGIN
    Out.Int(x, 0);
    IF ODD(x) THEN
        Out.String(" is odd.")
    ELSE
        Out.String(" is even.")
    END;
    Out.Ln
END Odd;

PROCEDURE Short;
VAR s: SHORTINT;
    i: INTEGER;
    l: LONGINT;
    f: REAL;
    d: LONGREAL;
BEGIN
    l := 47;
    i := SHORT(l);
    s := SHORT(i);
    ASSERT(s = l);
    d := 4.2;
    f := SHORT(d);
    ASSERT(f = d)
END Short;

PROCEDURE Long;
VAR s: SHORTINT;
    i: INTEGER;
    l: LONGINT;
    f: REAL;
    d: LONGREAL;
BEGIN
    s := 42;
    i := LONG(s);
    l := LONG(i);
    ASSERT(l = s);
    f := 4.2;
    d := LONG(f);
    ASSERT(d = f)
END Long;

PROCEDURE Entier;
VAR f: REAL;
    d: LONGREAL;
    l: LONGINT;
BEGIN
    f := 4.2;
    l := ENTIER(f);
    ASSERT(l = 4);
    f := -4.2;
    l := ENTIER(f);
    ASSERT(l = -5);
    d := 4.7;
    l := ENTIER(d);
    ASSERT(l = 4);
    d := -4.7;
    l := ENTIER(d);
    ASSERT(l = -5)
END Entier;

PROCEDURE Abs;
VAR s: SHORTINT;
    i: INTEGER;
    l: LONGINT;
    f: REAL;
    d: LONGREAL;
BEGIN
    s := -42;
    ASSERT(ABS(s) = -s);
    i := -47;
    ASSERT(ABS(i) = -i);
    l := -1893;
    ASSERT(ABS(l) = -l);
    f := -3.1415926;
    ASSERT(ABS(f) = -f);
    d := -226.1976;
    ASSERT(ABS(d) = -d)
END Abs;

PROCEDURE A(): INTEGER;
BEGIN
    RETURN 1
END A;

PROCEDURE B(): INTEGER;
BEGIN
    RETURN 0
END B;

BEGIN
    a := A();
    b := B();
    ASSERT(a > b);
    ASSERT(A() > B());
    ASSERT(A() + B() = 1);
    NewFree;
    IncDec;
    LslAsr;
    RolRor;
    Odd(a);
    Odd(b);
    ASSERT(~ODD(0));
    ASSERT(ODD(1));
    ASSERT(a > b);
    IF (a < b) THEN HALT(1) END;
    Short;
    Long;
    Entier;
    Abs;
    Out.String("Passed all assertions."); Out.Ln;
    HALT(0);
    Out.String("Error: Unreachable!")
END Predefines.