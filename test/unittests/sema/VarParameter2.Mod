(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE VarParameter2;

  TYPE
    Object = POINTER TO ObjectDesc;
    ObjectDesc = RECORD END;
    Shape = POINTER TO ShapeDesc;
    ShapeDesc = RECORD (ObjectDesc) area: PROCEDURE(s: Shape): REAL END;

  VAR
    s: SHORTINT;
    i: INTEGER;
    l: LONGINT;
    od: ObjectDesc;
    sd: ShapeDesc;
    op: Object;
    sp: Shape;

  PROCEDURE p1(VAR v: INTEGER);
  END p1;

  PROCEDURE p2(VAR o: ObjectDesc);
  END p2;

  PROCEDURE p3(VAR s: ShapeDesc);
  END p3;

  PROCEDURE p4(VAR o: Object);
  END p4;

  PROCEDURE p5(VAR s: Shape);
  END p5;

BEGIN
  p1(s);
  p1(i);
  p1(l);
  p2(od);
  p2(sd);
  p3(od);
  p3(sd);
  p4(op);
  p4(sp);
  p5(op);
  p5(sp)
END VarParameter2.
(*
  CHECK: {{.*}}:37:6:{{.*}}error:{{.*}}type mismatch: cannot pass an expression of type SHORTINT to variable parameter of type INTEGER.
  CHECK: {{.*}}:39:6:{{.*}}error:{{.*}}type mismatch: cannot pass an expression of type LONGINT to variable parameter of type INTEGER.
  CHECK: {{.*}}:42:6:{{.*}}error:{{.*}}type mismatch: ObjectDesc is not an extension of ShapeDesc.
  CHECK: {{.*}}:45:6:{{.*}}error:{{.*}}type mismatch: cannot pass an expression of type Shape to variable parameter of type Object.
  CHECK: {{.*}}:46:6:{{.*}}error:{{.*}}type mismatch: cannot pass an expression of type Object to variable parameter of type Shape.
*)