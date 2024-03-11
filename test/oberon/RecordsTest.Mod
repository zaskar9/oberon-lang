MODULE RecordsTest;
IMPORT Out;

TYPE Point2D* = RECORD x*, y*: INTEGER END;
     Point3D = RECORD
                   p2d: Point2D;
                   z: INTEGER
               END;
     Circle = RECORD
                  centre: RECORD x, y: INTEGER END;
                  radius: INTEGER
              END;

VAR point2d: Point2D;
    x, y: Point2D;
    point3d: Point3D;
    circle: Circle;

PROCEDURE TestNested(x, y: INTEGER; VAR p: Point2D);

    PROCEDURE TestAssignment(x, y: INTEGER; VAR p: Point2D);
    BEGIN
        p.x := x;
        p.y := y
    END TestAssignment;

BEGIN
    TestAssignment(x, y, p)
END TestNested;

PROCEDURE TestAssignment(x, y: INTEGER; VAR p: Point2D);
VAR point2d: Point2D;
    a, b: INTEGER;
BEGIN
    point2d.x := x;
    point2d.y := y;
    a := point2d.x;
    b := point2d.y;
    p := point2d
END TestAssignment;

BEGIN
    point2d.x := 0; point2d.y := 0;
    x := point2d;
    y := point2d;
    ASSERT(x.x = y.x);
    ASSERT(y.x = y.y);
    point3d.p2d.x := -1; point3d.p2d.y := -1; point3d.z := -1;
    point3d.p2d := point2d;
    ASSERT(point3d.p2d.x = point2d.x);
    ASSERT(point3d.p2d.y = point2d.y);
    circle.centre.x := 10; circle.centre.y := 10; circle.radius := 5;
    TestAssignment(42, 47, point2d);
    ASSERT(point2d.x = 42);
    ASSERT(point2d.y = 47);
    TestNested(47, 42, point2d);
    ASSERT(point2d.x = 47);
    ASSERT(point2d.y = 42);
    Out.String("All assertions passed."); Out.Ln
END RecordsTest.