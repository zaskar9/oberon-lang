MODULE PolymorphicReturnType;
IMPORT Math, Out, Shape, Object;

TYPE
  (* Object = POINTER TO ObjectDesc;
  ObjectDesc = RECORD END; *)

  (* Shape = POINTER TO ShapeDesc;
  ShapeDesc = RECORD (Object.ObjectDesc)
                area: PROCEDURE(this: Shape): REAL
              END; *)

  Circle = POINTER TO CircleDesc;
  CircleDesc = RECORD (Shape.ShapeDesc)
                 radius: REAL
               END;

VAR
  shape: Shape.Shape;

  PROCEDURE circleArea(this: Shape.Shape): REAL;
  VAR
    circle: Circle;
  BEGIN
    IF circle IS Shape.Shape THEN Out.String("Yes") END;
    circle := this(Circle);
    RETURN Math.pi * circle.radius * circle.radius
  END circleArea;

  PROCEDURE createCircle(radius: REAL): Shape.Shape;
  VAR
    circle: Circle;
  BEGIN
    NEW(circle);
    circle.area := circleArea;
    circle.radius := radius;
    RETURN circle
  END createCircle;

BEGIN
  shape := createCircle(2.5);
  Out.Real(shape.area(shape), 16); Out.Ln
END PolymorphicReturnType.
