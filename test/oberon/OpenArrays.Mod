MODULE OpenArrays;
IMPORT Out;

  CONST Dim = 10;

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
    len := SHORT(LEN(m, 0)) (* Dim *);
    FOR i := 0 TO len - 1 DO
      Out.Int(Get1D(m, i), 3)
    END;
    Out.Ln
  END PrintArray;

  PROCEDURE PrintMatrix(m: ARRAY OF ARRAY OF INTEGER);
  VAR i, len: INTEGER;
  BEGIN
    len := SHORT(LEN(m, 0)) (* Dim *);
    Out.Int(len, 0); Out.Char(","); Out.Long(LEN(m, 1), 0); Out.Ln;
    FOR i := 0 TO len - 1 DO
      PrintArray(m[i])
    END;
    Out.Ln
  END PrintMatrix;

  PROCEDURE PrintMatrix2(m: ARRAY OF ARRAY OF INTEGER);
  VAR i, j, len1, len2: INTEGER;
  BEGIN
    len1 := SHORT(LEN(m, 0)) (* Dim *);
    len1 := SHORT(LEN(m, 1)) (* Dim *);
    Out.Int(len1, 0); Out.Char(","); Out.Int(len2, 0); Out.Ln;
    FOR i := 0 TO len1 - 1 DO
      FOR j := 0 TO len2 - 1 DO
        Out.Int(Get2D(m, i, j), 3)
      END;
      Out.Ln
    END;
    Out.Ln
  END PrintMatrix2;

  PROCEDURE InitMatrix(VAR m: ARRAY OF ARRAY OF INTEGER);
  VAR i, j, len1, len2: INTEGER;
  BEGIN
    len1 := SHORT(LEN(m, 0)) (* Dim *);
    len2 := SHORT(LEN(m, 1)) (* Dim *);
    FOR i := 0 TO len1 - 1 DO
      FOR j := 0 TO len2 - 1 DO
        Set2D(m, i, j, len1 * i + j)
      END
    END
  END InitMatrix;

BEGIN
  Set1D(array, 9, 42);
  Out.Int(Get1D(array, 9), 3); Out.Ln;
  Set2D(matrix, 4, 5, 99);
  Out.Int(Get2D(matrix, 4, 5), 3); Out.Ln;
  InitMatrix(matrix);
  PrintMatrix2(matrix);
  i := 0;
  InitMatrix(cube[i]);
  PrintMatrix(cube[i]);
  PrintArray(cube[i, 5])
END OpenArrays.