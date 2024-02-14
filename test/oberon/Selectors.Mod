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
    person: Person;

BEGIN
    NEW(person);
    Out.String(person.spouse.title[0])
END Selectors.