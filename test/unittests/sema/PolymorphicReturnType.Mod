(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE PolymorphicReturnType;
IMPORT Math, Out;

TYPE
  Object = POINTER TO ObjectDesc;
  ObjectDesc = RECORD END;

  Shape = POINTER TO ShapeDesc;
  ShapeDesc = RECORD (ObjectDesc)
                area: PROCEDURE(this: Shape): REAL
              END;

  Circle = POINTER TO CircleDesc;
  CircleDesc = RECORD (ShapeDesc)
                 radius: REAL
               END;

VAR
  shape: Shape;

  PROCEDURE circleArea(this: Shape): REAL;
  VAR
    circle: Circle;
  BEGIN
    IF circle IS Shape THEN Out.String("Yes") ELSE Out.String("No") END; Out.Ln;
    circle := this(Circle);
    RETURN Math.pi * circle.radius * circle.radius
  END circleArea;

  PROCEDURE createCircle(radius: REAL): Shape;
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
  Out.Real(shape.area(shape), 16); Out.Ln;
  IF shape IS Object THEN Out.String("Yes") ELSE Out.String("No") END; Out.Ln
END PolymorphicReturnType.
(*
  CHECK: No
  CHECK:   1.96349552E+01
  CHECK: Yes
*)