MODULE RecordAssignment;

  TYPE
    Object = POINTER TO ObjectDesc;
    ObjectDesc = RECORD END;
    Point2d = POINTER TO Point2dDesc;
    Point2dDesc = RECORD (ObjectDesc) x, y: REAL; buffer: ARRAY 256 OF BYTE END;

  VAR
    o: Object;
    od: ObjectDesc;
    p: Point2d;
    pd: Point2dDesc;

  PROCEDURE AssignRec(VAR o: ObjectDesc; p: Point2dDesc);
  BEGIN
    o := p
  END AssignRec;

  PROCEDURE AssignPtr(o: Object; p: Point2d);
  BEGIN
    o := p
  END AssignPtr;

  PROCEDURE AssignMixed(VAR o: Object; p: Point2dDesc);
  BEGIN
    o^ := p
  END AssignMixed;

BEGIN
  NEW(p);
  p.x := 1.0; p.y := 1.0;
  o := p;
  AssignRec(od, p^);
  AssignPtr(o, p);
  AssignMixed(o, p^);
  od := pd
END RecordAssignment.