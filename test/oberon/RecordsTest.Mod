MODULE RecordsTest;

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
    point3d: Point3D;
    circle: Circle;

BEGIN
    point2d.x := 0; point2d.y := 0;
    point3d.p2d.x := 0; point3d.p2d.y := 0; point3d.z := 0;
    circle.centre.x := 10; circle.centre.y := 10; circle.radius := 5
END RecordsTest.