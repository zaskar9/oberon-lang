MODULE RecordExt;
IMPORT Out;

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
    Point2D = RECORD (ObjectDesc)
        x, y: INTEGER
    END;
    (* Point3D = RECORD (Point2D)
        z: INTEGER
    END;*)
    Shape = POINTER TO ShapeDesc;
    ShapeDesc = RECORD (ObjectDesc)
        area: REAL
    END;
    Circle = POINTER TO CircleDesc;
    CircleDesc* = RECORD (ShapeDesc)
        centre: Point2D;
        radius*: REAL
    END;
    (*Sphere = RECORD (ObjectDesc)
        centre: Point3D;
        radius: REAL
    END;*)

VAR (*nd: IntNodeDesc;*)
    o: Object;
    s: Shape;
    c: Circle;
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
    Out.String("[x: "); Out.Int(p.x, 0); Out.String(", y: "); Out.Int(p.y, 0); Out.Char("]")
END PrintPoint2D;

PROCEDURE PrintCircle(c: CircleDesc);
BEGIN
    Out.String("[centre: "); PrintPoint2D(c.centre);
    Out.String(", radius: "); Out.Real(c.radius, 0);
    Out.String(", area: "); Out.Real(c.area, 0); Out.Char("]")
END PrintCircle;

BEGIN
    (*nd.val := 0;
    Init(c, 3.141);*)
    cd.centre.x := 1;
    cd.centre.y := 1;
    cd.radius := 10.0;
    cd.area := 3.14159265359 * cd.radius * cd.radius;
    PrintCircle(cd); Out.Ln;

    NEW(c);
    c.centre.x := -1;
    c.centre.y := -1;
    c.radius := 1.0;
    c.area := 3.14159265359 * c.radius * c.radius;
    PrintCircle(c^); Out.Ln;
    FREE(c);
    (*NEW(inode);
    node := inode;*)
    (*inode := node;*)
    (*node(IntNode).val := 42*)
END RecordExt.