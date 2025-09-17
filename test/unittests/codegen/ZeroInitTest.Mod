(*
  RUN: %oberon --run %s
*)
MODULE ZeroInitTest;

TYPE
  Point* = POINTER TO PointDesc;
  PointDesc = RECORD x: INTEGER; y: ARRAY 10 OF REAL; z: INTEGER END;

VAR
  i: INTEGER;
  r: REAL;
  p: Point;
  pd: PointDesc;
  pList: ARRAY 10 OF Point;
  pdList: ARRAY 10, 10 OF PointDesc;

PROCEDURE Test;
VAR
  i: INTEGER;
  r: REAL;
  p: Point;
  pd: PointDesc;
  pList: ARRAY 10 OF Point;
  pdList: ARRAY 10, 10 OF PointDesc;
BEGIN
  ASSERT(i = 0);
  ASSERT(r = 0.0);
  ASSERT(p = NIL);
  ASSERT(pd.z = 0.0);
  ASSERT(pList[3] = NIL);
  ASSERT(pdList[3,9].z = 0.0)
END Test;

BEGIN
  ASSERT(i = 0);
  ASSERT(r = 0.0);
  ASSERT(p = NIL);
  ASSERT(pd.z = 0.0);
  ASSERT(pList[3] = NIL);
  ASSERT(pdList[3,9].z = 0.0);
  Test
END ZeroInitTest.