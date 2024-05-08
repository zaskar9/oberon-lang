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
    END;
    Point2D = RECORD (ObjectDesc)
        x, y: INTEGER
    END;
    Point3D = RECORD (Point2D)
        z: INTEGER
    END;*)
    Shape = POINTER TO ShapeDesc;
    ShapeDesc = RECORD (ObjectDesc)
        area: REAL
    END;
    Circle = POINTER TO CircleDesc;
    CircleDesc* = RECORD (ShapeDesc)
        (*centre: Point2D;*)
        x, y: INTEGER;
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

BEGIN
    (*nd.val := 0;
    Init(c, 3.141);*)
    NEW(c);
    c.x := -1;
    c.y := -1;
    c.radius := 1.0;
    c.area := 3.14159265359 * c.radius * c.radius;
    Out.Int(c.x, 0); Out.Ln;
    Out.Int(c.y, 0); Out.Ln;
    Out.Real(c.radius, 0); Out.Ln;
    Out.Real(c.area, 0); Out.Ln;
    (*FREE(c);*)
    (*NEW(inode);
    node := inode;*)
    (*inode := node;*)
    (*node(IntNode).val := 42*)
END RecordExt.