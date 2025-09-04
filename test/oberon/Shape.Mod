MODULE Shape;
IMPORT Object;

TYPE
  Shape* = POINTER TO ShapeDesc;
  ShapeDesc* = RECORD (Object.ObjectDesc)
                area*: PROCEDURE(this: Shape): REAL
              END;

END Shape.