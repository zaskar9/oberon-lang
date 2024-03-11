MODULE Chars;
IMPORT Out;

TYPE String = ARRAY 256 OF CHAR;
     ShortString = ARRAY 50 OF CHAR;
     PersonDesc = RECORD
        first: ShortString;
        last: ShortString
     END;
     Person = POINTER TO PersonDesc;
     People = ARRAY 10 OF Person;

VAR s1: ShortString;
    s2: String;
    s3: ShortString;
    s4: ARRAY 5 OF CHAR;
    i: INTEGER;
    ch: CHAR;
    p1, p2: Person;
    pd: PersonDesc;
    people: People;
    f: REAL;
    l: LONGINT;

PROCEDURE CreatePerson(first, last: ARRAY OF CHAR): Person;
VAR p: Person;
BEGIN
    NEW(p);
    p.first := first;
    p.last := last;
    RETURN p
END CreatePerson;

PROCEDURE PrintPerson(p: Person);
BEGIN
    Out.String(p.last); Out.String(", "); Out.String(p.first)
END PrintPerson;

PROCEDURE CreatePerson0(first, last: ARRAY OF CHAR; VAR p: PersonDesc);
BEGIN
    p.first := first;
    p.last := last
END CreatePerson0;

PROCEDURE PrintPerson0(p: PersonDesc);
BEGIN
    Out.String(p.last); Out.String(", "); Out.String(p.first)
END PrintPerson0;

PROCEDURE PrintPeople(people: ARRAY OF Person);
BEGIN
    PrintPerson(people[0]); Out.Ln;
    PrintPerson(people[1])
END PrintPeople;

PROCEDURE HelloWorld(VAR str: ARRAY OF CHAR);
BEGIN
    str := "Hello World!"
END HelloWorld;

BEGIN
    s1[0] := 4FX;
    s1[1] := 62X;
    s1[2] := "e";
    s1[3] := "r";
    s1[4] := "o";
    s1[5] := "n";
    s1[6] := "!";
    s1[7] := "\0";

    s2 := s1;
    s2[6] := "?";

    s3 := "Hello World!";

    Out.String("s1: "); Out.String(s1); Out.Ln;
    Out.String("s2: "); Out.String(s2); Out.Ln;
    Out.String("s3: "); Out.String(s3); Out.Ln;

    ASSERT(LEN("This is a test.") = 16);

    s3 := "\n";
    Out.String("["); Out.Char("]"); Out.String(s3);

    s4 := "supercalifragilisticexpialidocious";
    s4 := "otto";

    NEW(p1);
    p1.first := "Harry";
    p1.last := "Potter";
    people[0] := p1;
    p2 := CreatePerson("Hermione", "Granger");
    people[1] := p2;
    PrintPeople(people); Out.Ln;
    PrintPerson(people[0]); Out.Ln;
    PrintPerson(people[1]); Out.Ln;
    FREE(p2);
    FREE(p1);

    CreatePerson0("Hermione", "Granger", pd);
    PrintPerson0(pd); Out.Ln;

    pd.first := s1;
    pd.last := "Hello";
    PrintPerson0(pd); Out.Ln;

    pd.first[6] := CHR(63);
    PrintPerson0(pd); Out.Ln;

    HelloWorld(s1);
    s1[0] := "M";
    (*s1 := s1[0];*)
    Out.String(s1); Out.Ln;
    (*Out.String(s1[0]);*)
    Out.String("Hello World!"); Out.Ln;

    ASSERT(ORD("A") = 65);
    ASSERT(CHR(ORD("A")) = "A");
    ASSERT(ORD(s1[0]) = ORD("M"));

    Out.String("All assertions passed."); Out.Ln

END Chars.