MODULE RecordExt;
IMPORT Out;

TYPE
    ObjectDesc = RECORD END;
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD (ObjectDesc)
        int: INTEGER;
        next: Node
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

VAR nd: NodeDesc;
    circ: Circle;

PROCEDURE Init(VAR c: Shape; r: REAL);
BEGIN
    c(Circle).radius := r;
    c(Sphere).radius := r
END Init;

BEGIN
    nd.int := 0;
    Init(circ, 3.141);
    Out.Real(circ.radius, 0); Out.Ln
END RecordExt.