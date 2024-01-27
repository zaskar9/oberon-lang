MODULE Pointers;
IMPORT Out;

CONST KNZip* = 78457;
      NumContacts = 10;

TYPE IntPtr = POINTER TO INTEGER;
     AddressDesc = RECORD
                      street: STRING;
                      number: INTEGER;
                      city: STRING;
                      zipcode: INTEGER;
                      country: STRING
                   END;
     Address = POINTER TO AddressDesc;
     PersonDesc = RECORD
                     title: STRING;
                     name: STRING;
                     address: Address
                  END;
     Person = POINTER TO PersonDesc;
     ArrayPtr = POINTER TO ARRAY 20 OF INTEGER;
     Contacts = ARRAY NumContacts OF Person;

VAR ptr1, ptr2: IntPtr;
    michael: Person;
    ten*: INTEGER;
    array: ArrayPtr;
    contacts: Contacts;

PROCEDURE createPerson(title, name, street: STRING; number: INTEGER; city: STRING; zipcode: INTEGER; country: STRING): Person;
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

    (* --- Test pointer to base type -------------------------------------------------------------------------------- *)
    ptr1 := NIL;
    ptr2 := NIL;
    ASSERT(ptr1 = ptr2);
    ASSERT(ptr1 = NIL);
    IF (NIL = NIL) THEN Out.String("NIL = NIL"); Out.Ln END;
    (* ASSERT(NIL = NIL); <-- strangely, this does not work! *)
    NEW(ptr1);
    ASSERT(ptr1 # NIL);
    ptr1^ := 2 * ten;
    ptr2 := ptr1;
    ASSERT(ptr1 = ptr2);
    ASSERT(ptr2^ = ptr1^);
    ptr2^ := ten;
    ASSERT(ptr1^ = ptr2^);

    (* --- Test pointer to record type ------------------------------------------------------------------------------ *)
    michael := createPerson("Prof. Dr.", "Michael Grossniklaus", "Universitätsstraße", ptr1^, "Konstanz", KNZip, "Germany");
    contacts[0] := michael;
    contacts[1] := createPerson("Prof. Dr.", "Tobias Sutter", "Universitätsstraße", ptr2^, "Konstanz", KNZip, "Germany");
    printContacts(contacts);

    deletePerson(contacts[0]);
    deletePerson(contacts[1]);
    FREE(ptr1);
    ASSERT(ptr1 = NIL);

    (* --- Test pointer to array type ------------------------------------------------------------------------------- *)
    NEW(array);
    array[0] := ten;
    array[1] := ten;
    ASSERT(array[0] * array[1] = ten * ten);
    FREE(array)

END Pointers.