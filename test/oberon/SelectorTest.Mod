MODULE SelectorTest;
IMPORT Out;

TYPE String = ARRAY 255 OF CHAR;
     Triple = ARRAY 3 OF INTEGER;
     TripleTriple = ARRAY 3 OF Triple;
     TripleTripleTriple = ARRAY 3 OF TripleTriple;
     Triples = POINTER TO TriplesDesc;
     TriplesDesc = RECORD t: Triple; tt: TripleTriple; ttt: TripleTripleTriple END;
     Shape = POINTER TO ShapeDesc;
     ShapeDesc = RECORD area: REAL END;
     Circle = POINTER TO CircleDesc;
     CircleDesc = RECORD (ShapeDesc) x, y, r: REAL END;
     Person = POINTER TO PersonDesc;
     PersonDesc = RECORD first, last: String END;

VAR s: Shape;
    sd: ShapeDesc;
    c: Circle;
    cd: CircleDesc;
    p: Person;
    pd: PersonDesc;
    t: Triple;
    tt: TripleTriple;
    ttt: TripleTripleTriple;
    ts: Triples;
    tsd: TriplesDesc;
    x, y, z: INTEGER;
    tss: ARRAY 3, 3, 3 OF TriplesDesc;

PROCEDURE Dereference(VAR sd: ShapeDesc; s: Shape);
BEGIN
    sd := s^
END Dereference;

PROCEDURE Test;
VAR s: Shape;
    sd: ShapeDesc;
BEGIN
    NEW(s);
    s.area := 0.0;
    Dereference(sd, s);
    DISPOSE(s)
END Test;

PROCEDURE Print(str: ARRAY OF CHAR);
VAR i: LONGINT;
BEGIN
    i := 0;
    Out.Long(LEN(str), 0); Out.Char(":");
    WHILE (i < LEN(str)) & (str[i] # 0X) DO
        Out.Char(str[i]);
        INC(i)
    END
END Print;

PROCEDURE TrapArrayBounds1(VAR a: ARRAY OF INTEGER; x: INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(a, 0) - 1) DO a[i] := 42 END;
    Out.Int(x, 0); Out.Ln;
    a[x] := x
END TrapArrayBounds1;

PROCEDURE TrapArrayBounds2(VAR a: ARRAY OF ARRAY OF INTEGER; x, y: INTEGER);
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(a, 0) - 1) DO
        FOR j := 0 TO SHORT(LEN(a, 1) - 1) DO
            a[i,j] := 42
        END
    END;
    Out.Int(x, 0); Out.Char(","); Out.Int(y, 0); Out.Ln;
    a[x,y] := x + y
END TrapArrayBounds2;

PROCEDURE TrapArrayBounds3(VAR a: ARRAY OF ARRAY OF ARRAY OF INTEGER; x, y, z: INTEGER);
VAR i, j, k: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(a, 0) - 1) DO
        FOR j := 0 TO SHORT(LEN(a, 1) - 1) DO
            FOR k := 0 TO SHORT(LEN(a, 2) - 1) DO
                a[i][j][k] := 42
            END
        END
    END;
    Out.Int(x, 0); Out.Char(","); Out.Int(y, 0); Out.Char(","); Out.Int(z, 0); Out.Ln;
    a[x][y][z] := x + y + z
END TrapArrayBounds3;

PROCEDURE TrapArrayBounds(x, y, z: INTEGER);
VAR t: Triple;
    tt: TripleTriple;
    ttt: TripleTripleTriple;
    ts: Triples;
    tsd: TriplesDesc;
BEGIN
    NEW(ts);
    TrapArrayBounds1(t, x);
    TrapArrayBounds2(tt, x, y);
    TrapArrayBounds3(ttt, x, y, z);
    DISPOSE(ts)
END TrapArrayBounds;

BEGIN
    Out.String("--- Shape"); Out.Ln;
    NEW(s);
    s.area := 1.0;
    Out.Real(s.area, 0); Out.Ln;
    Dereference(sd, s);
    DISPOSE(s);
    Out.Real(sd.area, 0); Out.Ln;
    Out.String("--- Circle"); Out.Ln;
    NEW(c);
    c.area := 1.0;
    c.x := 2.0;
    c.y := 3.0;
    c.r := 4.0;
    Out.Real(c.area, 0); Out.Ln;
    Out.Real(c.x, 0); Out.Ln;
    Out.Real(c.y, 0); Out.Ln;
    Out.Real(c.r, 0); Out.Ln;
    Dereference(cd, c);
    cd := c^;
    DISPOSE(c);
    Out.Real(cd.area, 0); Out.Ln;
    Out.Real(cd.x, 0); Out.Ln;
    Out.Real(cd.y, 0); Out.Ln;
    Out.Real(cd.r, 0); Out.Ln;

    Out.String("--- Person"); Out.Ln;
    NEW(p);
    p.first := "John"; p.last := "Doe";
    Out.String(p.first); Out.Char(" "); Out.String(p.last); Out.Ln;
    (* Dereference(pd, p); *)
    pd := p^;
    DISPOSE(p);
    Out.String(pd.first); Out.Char(" "); Out.String(pd.last); Out.Ln;
    Print(pd.first); Out.Char(" "); Print(pd.last); Out.Ln;

    Out.String("--- Array Bounds"); Out.Ln;
    x := 0; y := 0; z := 0;
    TrapArrayBounds1(t, x);
    TrapArrayBounds2(tt, x, y);
    TrapArrayBounds3(ttt, x, y, z);
    TrapArrayBounds(x, y, z);

    tss[0,1,2].ttt[2,1][0] := 42;
    Out.Int(tss[0][1,2].ttt[2][1][0], 0); Out.Ln

END SelectorTest.