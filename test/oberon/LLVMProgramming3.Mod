MODULE LLVMProgramming3;

  CONST Dim = 20;

  TYPE Array = ARRAY Dim OF INTEGER;

  VAR a: Array;

  PROCEDURE Init;
    VAR i: INTEGER;
  BEGIN
    i := 0;
    WHILE i < Dim DO
      a[i] := Dim-i;
      i := i + 1
    END
  END Init;

  PROCEDURE Swap(VAR a, b: INTEGER);
    VAR t: INTEGER;
  BEGIN
    t := a; a := b; b := t
  END Swap;

  PROCEDURE BubbleSort;
    VAR i, j: INTEGER;
  BEGIN
    i := 0;
    WHILE i < Dim DO
      j := Dim - 1;
      WHILE j > i DO
        IF a[j - 1] > a[j] THEN Swap(a[j - 1], a[j]) END;
        j := j - 1
      END;
      i := i + 1
    END
  END BubbleSort;

  PROCEDURE CondExec;
    VAR i, j: INTEGER;
  BEGIN
    IF i > j THEN i := j ELSE j := i END
  END CondExec;

  PROCEDURE Loop;
    VAR i, j: INTEGER;
  BEGIN
    i := 0; j := 1;
    WHILE i < 10 DO
      j := j * i;
      i := i + 1
    END
  END Loop;

  PROCEDURE Case;
    VAR x, y: INTEGER;
        ch: CHAR;
  BEGIN
    x := 0; y := 0;
    CASE ch OF
        "n": x := -1
      | "s": x := 1
      | "w": y := -1
      | "e": y := 1
    END
  END Case;



BEGIN
  Init;
  BubbleSort
END LLVMProgramming3.