MODULE Demo;
VAR
  i*: INTEGER;
  l: LONGINT;
  f: REAL;
  b: BOOLEAN;
BEGIN
    i := 10;
    l := i;
    f := l / 3;
    b := (i - l < 10) & (i + l > 10)
END Demo.