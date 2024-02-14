MODULE Selectors;
IMPORT Out;

CONST
    Dim = 10;

TYPE
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD
        val: INTEGER;
        next: Node
    END;
    Vector = ARRAY Dim OF INTEGER;
    Person = POINTER TO PersonDesc;
    PersonDesc = RECORD
        name: STRING;
        title: ARRAY 3 OF STRING;
        zipcode: INTEGER;
        spouse: Person
    END;

VAR
    fred, mary: Person;

BEGIN
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
    Out.String(fred.spouse.title[0]); Out.Ln;
    Out.Long(LEN(fred.title), 0); Out.Ln
END Selectors.