MODULE Selectors;
IMPORT Out;

CONST
    Ten = 10;
    Two = 2;

TYPE
    String = ARRAY 255 OF CHAR;
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD
        val: INTEGER;
        next: Node
    END;
    Vector = ARRAY Ten OF INTEGER;
    Array = RECORD
        dim: INTEGER;
        vec: Vector
    END;
    Person = POINTER TO PersonDesc;
    PersonDesc = RECORD
        name: String;
        title: ARRAY Two OF String;
        address: Array;
        zipcode: INTEGER;
        spouse: Person
    END;
    Matrix = ARRAY Two OF Vector;
    NamedMatrix = RECORD
        name: String;
        matrix: Matrix
    END;
    NamedMatrixArray = ARRAY Two OF NamedMatrix;

VAR
    fred, mary: Person;
    v: Vector;
    p: PersonDesc;
    m: Matrix;
    nm: NamedMatrix;
    nma: NamedMatrixArray;

PROCEDURE TestLocal();
VAR v: Vector;
    p: PersonDesc;
BEGIN
    ASSERT(LEN(v) = Ten);
    ASSERT(LEN(p.title) = Two);
    p.address.vec[9] := 42;
    ASSERT(p.address.vec[9] = 42)
END TestLocal;

PROCEDURE Len(VAR a: Array): INTEGER;
VAR i: INTEGER;
BEGIN
    i := a.dim;
    RETURN i
END Len;

BEGIN
    v[3] := 42;                         ASSERT(v[3] = 42);
    ASSERT(LEN(v) = Ten);
    p.zipcode := 8280;                  ASSERT(p.zipcode = 8280);
    p.address.dim := 10;                ASSERT(p.address.dim = 10);
    p.address.vec[9] := 42;             ASSERT(p.address.vec[9] = 42);
    ASSERT(Len(p.address) = Ten);
    ASSERT(LEN(p.title) = Two);
    ASSERT(LEN(v) = LEN(p.address.vec));
    ASSERT(LEN(m) = Two);
    ASSERT(LEN(m[0]) = Ten);
    ASSERT(LEN(nm.matrix) = Two);
    nm.matrix[0][0] := -42;             ASSERT(nm.matrix[0][0] = -42);
    nm.matrix[Two - 1][Ten - 1] := 42;  ASSERT(nm.matrix[Two - 1][Ten - 1] = 42);
    ASSERT(LEN(nm.matrix[0]) = Ten);
    ASSERT(LEN(nma) = Two);
    ASSERT(LEN(nma[0].matrix) = Two);
    ASSERT(LEN(nma[0].matrix[0]) = Ten);
    Out.String("Assertions for global variables passed."); Out.Ln;
    TestLocal;
    Out.String("Assertions for local variables passed."); Out.Ln;
    NEW(fred);
    fred.name := "Fred Bloggs";
    fred.title[0] := "Mr";
    fred.zipcode := 1234;
    NEW(mary);
    mary.name := "Mary Black";
    mary.title[0] := "Mrs";
    mary.title[1] := "Dr";
    mary.zipcode := fred.zipcode;
    mary.spouse := fred;
    fred.spouse := mary;
    ASSERT(fred.spouse.zipcode = fred.zipcode);
    ASSERT(LEN(fred.title) = Two);
    ASSERT(LEN(fred.address.vec) = Ten);
    ASSERT(LEN(fred.spouse.title) = LEN(fred.title));
    Out.String("Assertions for pointer variables passed."); Out.Ln
END Selectors.