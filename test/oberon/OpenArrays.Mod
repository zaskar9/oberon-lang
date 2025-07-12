MODULE OpenArrays;
IMPORT Out;

  TYPE Matrix = ARRAY 10, 10 OF INTEGER;

  VAR matrix: Matrix;

  PROCEDURE Print(m: ARRAY OF ARRAY OF INTEGER);
  VAR i, j: INTEGER;
  BEGIN
    FOR i := 0 TO 9 DO
      Out.Int(i, 2); Out.Char(":");
      FOR j := 0 TO 9 DO
        Out.Int(m[i, j], 3)
      END;
      Out.Ln
    END;
    Out.Ln
  END Print;

  PROCEDURE Init(VAR m: ARRAY OF ARRAY OF INTEGER);
  VAR i, j: INTEGER;
  BEGIN
    FOR i := 0 TO 9 DO
      FOR j := 0 TO 9 DO
        m[i, j] := i + j
      END
    END
  END Init;

BEGIN
  Init(matrix);
  Print(matrix)
END OpenArrays.