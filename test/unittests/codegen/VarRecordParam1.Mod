(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE VarRecordParam1;
IMPORT Math, Out;

  TYPE
    Object = RECORD END;
    Shape = RECORD(Object) area: PROCEDURE(): REAL END;
    Circle = RECORD(Shape) x, y, r: REAL END;

  VAR
    s1, s2, s3: Shape;
    c1, c2: Circle;

  PROCEDURE CircleArea(): REAL;
  BEGIN
    RETURN 1.0
  END CircleArea;

  PROCEDURE Test1(VAR o: Object; s: Shape);
  BEGIN
    o := s
  END Test1;

  PROCEDURE Test2(VAR o: Object; VAR s: Object);
  BEGIN
    o := s
  END Test2;

BEGIN
  s1.area := CircleArea;
  Out.Real(s1.area(), 10); Out.Ln;
  Test1(s2, s1);
  Out.Real(s2.area(), 10); Out.Ln;
  Test2(s3, s1);
  Out.Real(s3.area(), 10); Out.Ln;
  c1.area := CircleArea; c1.x := 1.0; c1.y := 1.0; c1.r := Math.pi;
  Test2(c2, c1);
  Out.Real(c2.r, 10); Out.Ln
END VarRecordParam1.
(*
  CHECK:  1.000E+00
  CHECK:  1.000E+00
  CHECK:  1.000E+00
  CHECK:  3.142E+00
*)