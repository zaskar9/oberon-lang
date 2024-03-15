MODULE RecordExt;
IMPORT Out;

TYPE
    ObjectDesc = RECORD END;
    Node = POINTER TO NodeDesc;
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
    END;
    Shape = RECORD (ObjectDesc)
        area: REAL
    END;
    Circle = RECORD (Shape)
        centre: Point2D;
        radius: REAL
    END;
    Sphere = RECORD (ObjectDesc)
        centre: Point3D;
        radius: REAL
    END;

VAR nd: IntNodeDesc;
    circ: Circle;
    node: Node;
    inode: IntNode;

PROCEDURE Init(VAR c: Shape; r: REAL);
BEGIN
    c(Circle).radius := r;
    c(Sphere).radius := r
END Init;

BEGIN
    nd.int := 0;
    Init(circ, 3.141);
    Out.Real(circ.radius, 0); Out.Ln;
    NEW(inode);
    node := inode;
    node(IntNode).val := 42
END RecordExt.