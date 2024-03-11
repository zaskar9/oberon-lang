MODULE Pointers;
IMPORT Out;

CONST KNZip* = 78464;
      NumContacts = 10;

TYPE (* IntPtr = POINTER TO INTEGER; *)
     String = ARRAY 50 OF CHAR;
     Address = POINTER TO AddressDesc;
     AddressDesc = RECORD
                      street: String;
                      number: INTEGER;
                      city: String;
                      zipcode: INTEGER;
                      country: String
                   END;
     Person = POINTER TO PersonDesc;
     PersonDesc = RECORD
                     title: ARRAY 10 OF CHAR;
                     name: String;
                     address: Address
                  END;
     (* ArrayPtr = POINTER TO ARRAY 20 OF INTEGER; *)
     Contacts = ARRAY NumContacts OF Person;

VAR (* ptr1, ptr2: IntPtr; *)
    p1, p2: Person;
    ten*: INTEGER;
    (* array: ArrayPtr; *)
    contacts: Contacts;

PROCEDURE createPerson(title, name, street: ARRAY OF CHAR;
                       number: INTEGER;
                       city: ARRAY OF CHAR;
                       zipcode: INTEGER;
                       country: ARRAY OF CHAR): Person;
VAR person: Person;
    address: Address;
BEGIN
    NEW(person);
    person.title := title;
    person.name := name;
    NEW(address);
    person.address := address;
    address.street := street;
    address.number := number;
    address.city := city;
    address.zipcode := zipcode;
    address.country := country;
    RETURN person
END createPerson;

PROCEDURE deletePerson(person: Person);
BEGIN
    FREE(person.address);
    FREE(person)
END deletePerson;

PROCEDURE printPerson(person: Person);
BEGIN
    Out.String(person.title); Out.String(" "); Out.String(person.name); Out.Ln;
    Out.String(person.address.street); Out.String(" "); Out.Int(person.address.number, 0); Out.Ln;
    Out.Int(person.address.zipcode, 5); Out.String(" "); Out.String(person.address.city); Out.Ln;
    Out.String(person.address.country); Out.Ln
END printPerson;

PROCEDURE printContacts(contacts: Contacts);
VAR current: INTEGER;
BEGIN
    current := 0;
    WHILE (contacts[current] # NIL) DO
        Out.String("<"); Out.Int(current + 1, 0); Out.String(">"); Out.Ln;
        printPerson(contacts[current]);
        current := current + 1
    END
END printContacts;

BEGIN
    ten := 10;

    (* --- Test pointer to base type --------------------------------------------------------------------------------
    ptr1 := NIL;
    ptr2 := NIL;
    ASSERT(ptr1 = ptr2);
    ASSERT(ptr1 = NIL);
    ASSERT(NIL = NIL);
    NEW(ptr1);
    ASSERT(ptr1 # NIL);
    ptr1^ := 2 * ten;
    ptr2 := ptr1;
    ASSERT(ptr1 = ptr2);
    ASSERT(ptr2^ = ptr1^);
    ptr2^ := ten;
    ASSERT(ptr1^ = ptr2^); *)

    (* --- Test pointer to record type ------------------------------------------------------------------------------ *)
    p1 := NIL;
    p2 := NIL;
    ASSERT(p1 = p2);
    ASSERT(p1 = NIL);
    ASSERT(NIL = NIL);
    NEW(p1);
    ASSERT(p1 # NIL);
    FREE(p1);
    ASSERT(p1 = NIL);
    p1 := createPerson("Prof. Dr.", "Michael Grossniklaus", "Universitätsstraße", 10, "Konstanz", KNZip, "Germany");
    ASSERT(p2 = NIL);
    p2 := p1;
    ASSERT(p2 # NIL);
    ASSERT(p1 = p2);
    contacts[0] := p1;
    contacts[1] := createPerson("Prof. Dr.", "Tobias Sutter", "Universitätsstraße", 10, "Konstanz", KNZip, "Germany");
    printContacts(contacts);
    deletePerson(contacts[0]);
    deletePerson(contacts[1])

    (* --- Test pointer to array type -------------------------------------------------------------------------------
    NEW(array);
    array[0] := ten;
    array[1] := ten;
    ASSERT(array[0] * array[1] = ten * ten);
    FREE(array) *)

END Pointers.