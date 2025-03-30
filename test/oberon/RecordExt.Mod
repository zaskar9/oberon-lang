MODULE RecordExt;
IMPORT Out, Math, Texts;

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

    MyWriterDesc = RECORD (Texts.Writer) flag: INTEGER END;
    MyWriter = POINTER TO MyWriterDesc;

    Anonymous = POINTER TO RECORD (ShapeDesc) a, b, c: INTEGER END;

VAR o: Object;
    s: Shape;
    c: Circle;
    p: Sphere;
    cd: CircleDesc;
    sl: ShapeList;
    sr: ShapeRecord;
    wtr: MyWriter;
    w: Texts.Writer;
    anon: Anonymous;

PROCEDURE PrintPoint2D(p: Point2D);
BEGIN
    Out.String("{ x: "); Out.Int(p.x, 0); Out.String(", y: "); Out.Int(p.y, 0); Out.String(" }")
END PrintPoint2D;

PROCEDURE PrintPoint3D(p: Point3D);
BEGIN
    Out.String("{ x: "); Out.Int(p.x, 0); Out.String(", y: "); Out.Int(p.y, 0); Out.String(", z: "); Out.Int(p.z, 0); Out.String(" }")
END PrintPoint3D;

PROCEDURE OutShapeDesc(sd: ShapeDesc);
BEGIN
    Out.String("{ area: "); Out.Real(cd.area, 13); Out.String(" }")
END OutShapeDesc;

PROCEDURE OutCircleDesc(cd: CircleDesc);
BEGIN
    Out.String("{ centre: "); PrintPoint2D(cd.centre);
    Out.String(", radius: "); Out.Real(cd.radius, 13);
    Out.String(", area: "); Out.Real(cd.area, 13); Out.String(" }")
END OutCircleDesc;

PROCEDURE OutSphereDesc(cd: SphereDesc);
BEGIN
    Out.String("{ centre: "); PrintPoint3D(cd.centre);
    Out.String(", radius: "); Out.Real(cd.radius, 13);
    Out.String(", area: "); Out.Real(cd.area, 13); Out.String(" }")
END OutSphereDesc;

PROCEDURE PrintShapeDesc(VAR sd: ShapeDesc);
BEGIN
    IF sd IS SphereDesc THEN
        Out.String("SphereDesc"); OutSphereDesc(sd(SphereDesc))
    ELSIF sd IS CircleDesc THEN
        Out.String("CircleDesc"); OutCircleDesc(sd(CircleDesc))
    ELSE
        Out.String("ShapeDesc"); OutShapeDesc(sd)
    END
END PrintShapeDesc;

PROCEDURE PrintShape(s: Shape);
BEGIN
    IF s IS Sphere THEN
        Out.String("Sphere"); OutSphereDesc(s(Sphere)^)
    ELSIF s IS Circle THEN
        Out.String("Circle"); OutCircleDesc(s(Circle)^)
    ELSE
        Out.String("Shape"); OutShapeDesc(s^)
    END
END PrintShape;

(* PROCEDURE PrintShapeList(VAR list: ARRAY OF Shape);
TYPE ptr = POINTER TO Point2D;
BEGIN
    CASE list[0] OF
        Circle: Out.String("X")
      | CircleDesc: Out.String("Y")
      | ptr: Out.String("Z")
    END
END PrintShapeList; *)

PROCEDURE PrintShapeDescCase(VAR sd: ShapeDesc);
BEGIN
    CASE sd OF
        CircleDesc: Out.String("CircleDesc"); OutCircleDesc(sd)
      | SphereDesc: Out.String("SphereDesc"); OutSphereDesc(sd)
      | ShapeDesc: Out.String("ShapeDesc"); OutShapeDesc(sd)
    END
END PrintShapeDescCase;

PROCEDURE PrintShapeCase(s: Shape);
BEGIN
    CASE s OF
        Circle: Out.String("Circle"); OutCircleDesc(s^)
      | Sphere: Out.String("Sphere"); OutSphereDesc(s^)
      | Shape: Out.String("Shape"); OutShapeDesc(s^)
    END
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
BEGIN
    NEW(e);
    e.id := 0;
    o := e;
    ASSERT(o IS Element);
    DISPOSE(e);
    NEW(o);
    ASSERT(o IS Object);
    ASSERT(~(o IS Element));
    DISPOSE(o)
END Test;

PROCEDURE TestExt(VAR w: Texts.Writer);
BEGIN
    IF w IS MyWriterDesc THEN
        Out.String("MyWriterDesc")
    ELSIF w IS Texts.Writer THEN
        Out.String("Texts.Writer")
    ELSE
        Out.String("Other")
    END
END TestExt;

BEGIN
    Test();
    NEW(anon);
    ASSERT(anon IS Anonymous);
    ASSERT(anon IS Shape);
    PrintShape(anon); Out.Ln;
    s := anon;
    PrintShape(s); Out.Ln;
    ASSERT(~(s IS Sphere));
    DISPOSE(anon);
    NEW(s);
    PrintShape(s); Out.Ln;
    DISPOSE(s);
    NEW(wtr);
    ASSERT(wtr IS MyWriter);
    TestExt(wtr^); Out.Ln;
    TestExt(w); Out.Ln;
    DISPOSE(wtr);
    (* cd.centre.x := 1;
    cd.centre.y := 1;
    cd.radius := 10.0;
    cd.area := CircleArea(cd.radius);
    PrintCircle(cd); Out.Ln; *)

    NEW(c);
    c.centre.x := -1;
    c.centre.y := -1;
    c.radius := 1.0;
    c.area := CircleArea(c.radius);

    (* PrintCircle(c^); Out.Ln;
    c^ := cd;
    c.radius := 5.0;
    c.area := CircleArea(c.radius);
    PrintCircle(c^); Out.Ln; *)

    (* o := c;
    ASSERT(o IS Circle);
    o(Circle).area := 0.1;
    PrintShape(o(Circle)); Out.Ln; *)

    (* PrintShapeDesc(c^); Out.Ln;
    ASSERT(o IS Shape);
    ASSERT(o IS Circle);
    ASSERT(~(o IS Sphere));
    sl[5] := c;
    ASSERT(sl[5] IS Circle);
    IF sl[5] IS Circle THEN PrintCircle(sl[5](Circle)^); Out.Ln END;
    sr.s := c;
    ASSERT(sr.s IS Circle);
    IF sr.s IS Circle THEN PrintCircle(sr.s(Circle)^); Out.Ln END; *)

    PrintShapeCase(c); Out.Ln;
    DISPOSE(c);

    (* Test();
    NEW(o);
    ASSERT(o IS Object);
    DISPOSE(o);
    ASSERT(~(o IS Object)); *)
    Out.String("All assertions passed."); Out.Ln
END RecordExt.