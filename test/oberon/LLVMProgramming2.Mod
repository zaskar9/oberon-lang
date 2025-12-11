MODULE LLVMProgramming2;

  VAR i, j: INTEGER;

  PROCEDURE ["C"] PrintF(str: ARRAY OF CHAR; ...); EXTERNAL [ "printf" ];

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

  PROCEDURE Test;
  VAR i, j: INTEGER;
  BEGIN
    i := Sum(i, j)
  END Test;

  PROCEDURE WriteInt(val: INTEGER);
  BEGIN
    PrintF("%d", val)
  END WriteInt;

BEGIN
  PrintF("Hello World!");
  i := 10; j := 0;
  Swap(i, j);
  i := Sum(i, j)
END LLVMProgramming2.