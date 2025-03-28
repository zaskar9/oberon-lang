MODULE RecordExt;
IMPORT Out, Math;

TYPE
    ObjectDesc = RECORD END;
    Object = POINTER TO ObjectDesc;
    Point2D* = RECORD (ObjectDesc)
        x*, y*: INTEGER
    END;
    Point3D = RECORD (Point2D)
        z: INTEGER
    END;
    Shape* = POINTER TO ShapeDesc;
    ShapeDesc = RECORD (ObjectDesc)
        area: REAL
    END;
    Circle* = POINTER TO CircleDesc;
    CircleDesc = RECORD (ShapeDesc)
        centre: Point2D;
        radius: REAL
    END;
    (* Sphere = POINTER TO RECORD (ShapeDesc) *)
    Sphere = POINTER TO SphereDesc;
    SphereDesc = RECORD (ShapeDesc)
        centre: Point3D;
        radius: REAL
    END;
    ShapeList = ARRAY 10 OF Shape;
    ShapeRecord = RECORD a, b, c, e: INTEGER; s: Shape END;

VAR o: Object;
    s: Shape;
    c: Circle;
    p: Sphere;
    cd: CircleDesc;
    sl: ShapeList;
    sr: ShapeRecord;

PROCEDURE PrintPoint2D(p: Point2D);
BEGIN
    Out.String("{ x: "); Out.Int(p.x, 0); Out.String(", y: "); Out.Int(p.y, 0); Out.String(" }")
END PrintPoint2D;

PROCEDURE PrintCircle(cd: CircleDesc);
BEGIN
    Out.String("{ centre: "); PrintPoint2D(cd.centre);
    Out.String(", radius: "); Out.Real(cd.radius, 13);
    Out.String(", area: "); Out.Real(cd.area, 13); Out.String(" }")
END PrintCircle;

PROCEDURE PrintShapeDesc(VAR sd: ShapeDesc);
BEGIN
    IF sd IS SphereDesc THEN
        Out.String("SphereDesc")
    ELSIF sd IS CircleDesc THEN
        Out.String("CircleDesc"); PrintCircle(sd(CircleDesc))
    ELSE
        Out.String("ShapeDesc")
    END
END PrintShapeDesc;

PROCEDURE PrintShape(s: Shape);
BEGIN
    IF s IS Sphere THEN
        Out.String("Sphere")
    ELSIF s IS Circle THEN
        Out.String("Circle"); PrintCircle(s(Circle)^)
    ELSE
        Out.String("Shape")
    END
END PrintShape;

PROCEDURE PrintShapeList(VAR list: ARRAY OF Shape);
TYPE ptr = POINTER TO Point2D;
BEGIN
    CASE list[0] OF
        Circle: Out.String("X")
      | CircleDesc: Out.String("Y")
      | ptr: Out.String("Z")
    END
END PrintShapeList;

PROCEDURE PrintShapeDescCase(VAR sd: ShapeDesc);
BEGIN
    CASE sd OF
        CircleDesc: Out.String("CircleDesc"); PrintCircle(sd)
      | SphereDesc: Out.String("SphereDesc")
      | ShapeDesc: Out.String("ShapeDesc")
    END
END PrintShapeDescCase;

PROCEDURE PrintShapeCase(s: Shape);
BEGIN
    CASE s OF
        Circle: Out.String("Circle"); PrintCircle(s^)
      | Sphere: Out.String("Sphere")
      | Shape: Out.String("Shape")
    END;
    Out.Int(s.centre.z, 0)
END PrintShapeCase;

PROCEDURE CircleArea(radius: REAL): REAL;
BEGIN
    RETURN Math.pi * radius * radius
END CircleArea;

PROCEDURE Test();
    TYPE Object = POINTER TO ObjectDesc;
         ObjectDesc = RECORD id: INTEGER END;
         Element = POINTER TO ElementDesc;
         ElementDesc = RECORD (ObjectDesc) val: INTEGER END;
    VAR o: Object;
        e: Element;
        od: ObjectDesc;
BEGIN
    NEW(e);
    e.id := 0;
    o := e;
    ASSERT(o IS Element);
    DISPOSE(e)
END Test;

BEGIN
    cd.centre.x := 1;
    cd.centre.y := 1;
    cd.radius := 10.0;
    cd.area := CircleArea(cd.radius);
    PrintCircle(cd); Out.Ln;
    NEW(c);
    c.centre.x := -1;
    c.centre.y := -1;
    c.radius := 1.0;
    c.area := CircleArea(c.radius);
    PrintCircle(c^); Out.Ln;
    c^ := cd;
    c.radius := 5.0;
    c.area := CircleArea(c.radius);
    PrintCircle(c^); Out.Ln;
    o := c;
    PrintShape(c); Out.Ln;
    PrintShapeDesc(c^); Out.Ln;
    ASSERT(o IS Shape);
    ASSERT(o IS Circle);
    ASSERT(~(o IS Sphere));
    sl[5] := c;
    ASSERT(sl[5] IS Circle);
    IF sl[5] IS Circle THEN PrintCircle(sl[5](Circle)^); Out.Ln END;
    sr.s := c;
    ASSERT(sr.s IS Circle);
    IF sr.s IS Circle THEN PrintCircle(sr.s(Circle)^); Out.Ln END;
    DISPOSE(c);
    Test();
    NEW(o);
    ASSERT(o IS Object);
    DISPOSE(o);
    ASSERT(~(o IS Object));
    Out.String("All assertions passed."); Out.Ln
END RecordExt.