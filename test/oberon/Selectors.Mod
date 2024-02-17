MODULE Selectors;
IMPORT Out;

CONST
    Ten = 10;
    Two = 2;

TYPE
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
        name: STRING;
        title: ARRAY Two OF STRING;
        address: Array;
        zipcode: INTEGER;
        spouse: Person
    END;
    Matrix = ARRAY Two OF Vector;
    NamedMatrix = RECORD
        name: STRING;
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

BEGIN
    v[0] := 42;
    ASSERT(LEN(v) = Ten);
    ASSERT(LEN(p.title) = Two);
    ASSERT(LEN(v) = LEN(p.address.vec));
    ASSERT(LEN(m) = Two);
    ASSERT(LEN(m[0]) = Ten);
    ASSERT(LEN(nm.matrix) = Two);
    nm.matrix[0][0] := -42;
    ASSERT(nm.matrix[0][0] = -42);
    nm.matrix[Two - 1][Ten - 1] := 42;
    ASSERT(nm.matrix[Two - 1][Ten - 1] = 42);
    ASSERT(LEN(nm.matrix[Two - 1]) = Ten);
    ASSERT(LEN(nma) = Two);
    ASSERT(LEN(nma[0].matrix) = Two);
    ASSERT(LEN(nma[0].matrix[0]) = Ten);
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
    ASSERT(LEN(v) = 10);
    ASSERT(LEN(fred.title) = Two);
    Out.String("Assertions passed."); Out.Ln
END Selectors.