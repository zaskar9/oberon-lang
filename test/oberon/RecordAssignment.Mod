MODULE RecordAssignment;

  IMPORT
    Out;

  TYPE
    Object = POINTER TO ObjectDesc;
    ObjectDesc = RECORD END;
    Point2d = POINTER TO Point2dDesc;
    Point2dDesc = RECORD (ObjectDesc) x, y: REAL; buffer: ARRAY 256 OF BYTE END;
    Person = POINTER TO PersonDesc;
    PersonDesc = RECORD (ObjectDesc) first, last: ARRAY 32 OF CHAR END;

  VAR
    o: Object;
    od: ObjectDesc;
    p: Point2d;
    pd: Point2dDesc;
    person: Person;

  PROCEDURE AssignRec(VAR o: ObjectDesc; p: Point2dDesc);
  BEGIN
    o := p
  END AssignRec;

  PROCEDURE AssignPtr(VAR o: Object; p: Point2d);
  BEGIN
    o := p
  END AssignPtr;

  PROCEDURE AssignMixed(o: Object; p: Point2dDesc);
  BEGIN
    o^ := p
  END AssignMixed;

BEGIN
  NEW(p);
  p.x := 1.0; p.y := 1.0;
  o := p;
  ASSERT(o IS Point2d);
  AssignRec(od, p^);
  AssignPtr(o, p);

  NEW(person);
  person.first := "Mabel";
  person.last := "Mora";
  AssignPtr(person, p);
  Out.String(person.last); Out.String(", "); Out.String(person.first); Out.Ln;
  AssignMixed(o, p^);
  od := pd
END RecordAssignment.