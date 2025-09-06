(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE OpenArrays;
IMPORT Out, SYSTEM;

  CONST Dim = 5;

  TYPE Array = ARRAY Dim OF INTEGER;
       Matrix = ARRAY Dim, Dim OF INTEGER;
       Cube = ARRAY Dim, Dim, Dim OF INTEGER;

  VAR array: Array;
      matrix: Matrix;
      cube: Cube;
      i: INTEGER;

  PROCEDURE Set1D(VAR m: ARRAY OF INTEGER; i, val: INTEGER);
  BEGIN
    m[i] := val
  END Set1D;

  PROCEDURE Get1D(m: ARRAY OF INTEGER; i: INTEGER): INTEGER;
  BEGIN
    RETURN m[i]
  END Get1D;

  PROCEDURE Set2D(VAR m: ARRAY OF ARRAY OF INTEGER; i, j, val: INTEGER);
  BEGIN
    m[i, j] := val
  END Set2D;

  PROCEDURE Get2D(m: ARRAY OF ARRAY OF INTEGER; i, j: INTEGER): INTEGER;
  BEGIN
    RETURN m[i, j]
  END Get2D;

  PROCEDURE Set3D(VAR c: ARRAY OF ARRAY OF ARRAY OF INTEGER; i, j, k, val: INTEGER);
  BEGIN
    c[i, j, k] := val
  END Set3D;

  PROCEDURE Get3D(c: ARRAY OF ARRAY OF ARRAY OF INTEGER; i, j, k: INTEGER): INTEGER;
  BEGIN
    RETURN c[i, j, k]
  END Get3D;

  PROCEDURE PrintArray(m: ARRAY OF INTEGER);
  VAR i, len: INTEGER;
  BEGIN
    len := SHORT(LEN(m, 0));
    FOR i := 0 TO len - 1 DO
      Out.Int(Get1D(m, i), 3)
    END;
    Out.Ln
  END PrintArray;

  PROCEDURE PrintMatrix(m: ARRAY OF ARRAY OF INTEGER);
  VAR i, len: INTEGER;
  BEGIN
    len := SHORT(LEN(m, 0));
    Out.Int(len, 0); Out.Char(","); Out.Long(LEN(m, 1), 0); Out.Ln;
    FOR i := 0 TO len - 1 DO
      PrintArray(m[i])
    END;
    Out.Ln
  END PrintMatrix;

  PROCEDURE PrintMatrix2(m: ARRAY OF ARRAY OF INTEGER);
  VAR i, j, len1, len2: INTEGER;
  BEGIN
    len1 := SHORT(LEN(m, 0));
    len1 := SHORT(LEN(m, 1));
    Out.Int(len1, 0); Out.Char(","); Out.Int(len2, 0); Out.Ln;
    FOR i := 0 TO len1 - 1 DO
      FOR j := 0 TO len2 - 1 DO
        Out.Int(Get2D(m, i, j), 3)
      END;
      Out.Ln
    END;
    Out.Ln
  END PrintMatrix2;

  PROCEDURE InitArray(VAR a: ARRAY OF INTEGER);
  VAR i, len: INTEGER;
  BEGIN
    len := SHORT(LEN(a));
    FOR i := 0 TO len - 1 DO
      Set1D(a, i, i)
    END
  END InitArray;

  PROCEDURE InitMatrix(VAR m: ARRAY OF ARRAY OF INTEGER);
  VAR i, j, len1, len2: INTEGER;
  BEGIN
    len1 := SHORT(LEN(m, 0));
    len2 := SHORT(LEN(m, 1));
    FOR i := 0 TO len1 - 1 DO
      FOR j := 0 TO len2 - 1 DO
        Set2D(m, i, j, len1 * i + j)
      END
    END
  END InitMatrix;

  PROCEDURE FlattenMatrix(VAR m: ARRAY OF ARRAY OF INTEGER);
  TYPE
    Vector = ARRAY Dim * Dim OF INTEGER;
  VAR
    v: Vector;
    i: INTEGER;
  BEGIN
    v := SYSTEM.VAL(Vector, m);
    FOR i := 0 TO Dim * Dim - 1 DO
      Out.Int(Get1D(v, i), 3)
    END;
    Out.Ln
  END FlattenMatrix;

  PROCEDURE FlattenCube();
  CONST
    Dim1 = 3;
    Dim3 = Dim1 * Dim1 * Dim1;
  TYPE
    Cube = ARRAY Dim1, Dim1, Dim1 OF INTEGER;
    Vector = ARRAY Dim3 OF INTEGER;
  VAR
    c: Cube;
    v: Vector;
    i, j, k, val: INTEGER;
  BEGIN
    val := 0;
    FOR i := 0 TO Dim1 - 1 DO
      FOR j := 0 TO Dim1 - 1 DO
        FOR k := 0 TO Dim1 - 1 DO
          Set3D(c, i, j, k, val);
          INC(val)
        END
      END
    END;
    v := SYSTEM.VAL(Vector, c);
    FOR i := 0 TO Dim3 - 1 DO Out.Int(Get1D(v, i), 3) END; Out.Ln
  END FlattenCube;

  PROCEDURE AssertDimensions1D(a: ARRAY OF INTEGER; dim: INTEGER);
  BEGIN
    ASSERT(LEN(a) = dim);
    ASSERT(LEN(a, 0) = dim)
  END AssertDimensions1D;

  PROCEDURE AssertDimensions2D(m: ARRAY OF ARRAY OF INTEGER; dim0, dim1: INTEGER);
  BEGIN
    ASSERT(LEN(m, 0) = dim0);
    ASSERT(LEN(m, 1) = dim1)
  END AssertDimensions2D;

  PROCEDURE AssertDimensions3D(c: ARRAY OF ARRAY OF ARRAY OF INTEGER; dim0, dim1, dim2: INTEGER);
  BEGIN
    ASSERT(LEN(c, 0) = dim0);
    ASSERT(LEN(c, 1) = dim1);
    ASSERT(LEN(c, 2) = dim2)
  END AssertDimensions3D;

BEGIN
  AssertDimensions1D(array, Dim);

  Set1D(array, 3, 42);
  ASSERT(Get1D(array, 3) = 42);

  AssertDimensions1D(matrix[0], Dim);
  AssertDimensions2D(matrix, Dim, Dim);

  Set2D(matrix, 1, 3, 99);
  ASSERT(Get2D(matrix, 1, 3) = 99);
  InitMatrix(matrix);
  FlattenMatrix(matrix);

  AssertDimensions1D(cube[0, 0], Dim);
  AssertDimensions2D(cube[0], Dim, Dim);
  AssertDimensions3D(cube, Dim, Dim, Dim);
  Set3D(cube, 0, 4, 2, 27);
  ASSERT(Get3D(cube, 0, 4, 2) = 27);
  FlattenCube
END OpenArrays.
(*
  CHECK:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
  CHECK:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
*)