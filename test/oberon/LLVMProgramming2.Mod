MODULE LLVMProgramming2;

  TYPE
    Vector = ARRAY 100 OF INTEGER;
    Matrix = ARRAY 10, 10 OF INTEGER;
    Point2D = RECORD x, y: INTEGER END;
    Triangle = ARRAY 3 OF Point2D;

  VAR
    i, j: INTEGER;
    v: Vector;
    m: Matrix;
    p: Point2D;
    t: Triangle;

  PROCEDURE ["C"] PrintF(str: ARRAY OF CHAR; ...); EXTERNAL [ "printf" ];

  PROCEDURE WriteInt(val: INTEGER);
  BEGIN
    PrintF("%d", val)
  END WriteInt;

  PROCEDURE WriteLn;
  BEGIN
    PrintF("\n")
  END WriteLn;

  PROCEDURE Swap(VAR i, j: INTEGER);
  VAR t: INTEGER;
  BEGIN
    t := i;
    i := j;
    j := t
  END Swap;

  PROCEDURE Sum(x, y: INTEGER): INTEGER;
  BEGIN
    RETURN x + y
  END Sum;

BEGIN
  t[2].x := 3;
  t[2].y := t[2].x;
  WriteInt(t[2].y);
  WriteLn
END LLVMProgramming2.