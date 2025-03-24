MODULE RecordExt;
IMPORT Out, Math;

TYPE
    Object = POINTER TO ObjectDesc;
    ObjectDesc = RECORD END;
    (*Node = POINTER TO NodeDesc;
    NodeDesc = RECORD (ObjectDesc)
        next: Node
    END;
    IntNode = POINTER TO IntNodeDesc;
    IntNodeDesc = RECORD (NodeDesc)
        val: INTEGER
    END;*)
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

VAR (*nd: IntNodeDesc;*)
    o: Object;
    s: Shape;
    c: Circle;
    p: Sphere;
    cd: CircleDesc;
    (*node: Node;
    inode: IntNode;*)

PROCEDURE Init(VAR c: Shape; r: REAL);
BEGIN
    (*c(Circle).radius := r;*)
    (*c(Sphere).radius := r;*)
END Init;

PROCEDURE PrintPoint2D(p: Point2D);
BEGIN
    Out.String("{ x: "); Out.Int(p.x, 0); Out.String(", y: "); Out.Int(p.y, 0); Out.String(" }")
END PrintPoint2D;

PROCEDURE PrintCircle(cd: CircleDesc);
BEGIN
    Out.String("{ centre: "); PrintPoint2D(cd.centre);
    Out.String(", radius: "); Out.Real(cd.radius, 0);
    Out.String(", area: "); Out.Real(cd.area, 0); Out.String(" }")
END PrintCircle;

PROCEDURE PrintShape(s: Shape);
BEGIN
    IF s IS Sphere THEN
        Out.String("Sphere")
        (* PrintCircle(s(Circle)) *)
    ELSIF s IS Circle THEN
        Out.String("Circle")
    ELSE
        Out.String("Shape")
    END
END PrintShape;
(*
PROCEDURE PrintShape2(s: Shape);
BEGIN
    CASE s OF
        Shape: Out.String("Shape")
      | Circle: Out.String("Circle")
      | Sphere: Out.String("Sphere")
    END
END PrintShape2;

PROCEDURE Test2(i: INTEGER);
BEGIN
    CASE i OF
        0: Out.String("zero") |
     1..9: Out.String("digit") |
       10: Out.String("ten") |
       11: Out.String("eleven") |
       12: Out.String("twelve") |
   13..19: Out.String("teen")
    END
END Test2;
*)
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
    (*nd.val := 0;
    Init(c, 3.141);*)
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
    ASSERT(o IS Shape);
    ASSERT(o IS Circle);
    ASSERT(~(o IS Sphere));
    DISPOSE(c);
    Test();
    (*NEW(inode);
    node := inode;*)
    (*inode := node;*)
    (*node(IntNode).val := 42*)
    Out.String("All assertions passed."); Out.Ln
END RecordExt.