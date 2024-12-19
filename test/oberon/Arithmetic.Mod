MODULE Arithmetic;
IMPORT Out;

PROCEDURE p1(x, y: INTEGER);
VAR i, j: LONGINT;
    r, s: REAL;
BEGIN
    i := x; j := y;
    r := i; s := j;
    i := j * j;
    r := s * s;
    Out.Long(i, 0); Out.Ln;
    Out.Real(r, 0); Out.Ln
END p1;

PROCEDURE p2(x, y: INTEGER);
VAR i: INTEGER;
BEGIN
    i := y;
    IF (x > y) & (i < 0) THEN i := x END;
    Out.Int(i, 0); Out.Ln
END p2;

BEGIN
    p1(1, 1);
    p2(1, 1)
END Arithmetic.
