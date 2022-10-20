MODULE Floats;
IMPORT Out;

CONST MAX_INTEGER = 2147483647;
      MIN_INTEGER = -2147483648;
      MAX_REAL = 3.40282E+38;
      MIN_REAL = 1.17549E-38;

VAR f1, f2: REAL;
    d1, d2: LONGREAL;
    i1, i2: INTEGER;
    l1, l2: LONGINT;

BEGIN
    i1 := 1;
    i2 := 2;
    l1 := i1;
    (* i2 := l2; *)
    d1 := f1;
    (* f2 := d2;
    f2 := l1; *)
    f1 := 3.141592654;
    f1 := i1;
    f2 := i1 + i2;
    f2 := i1 - f2;
    f2 := f1 * i2;
    f2 := f1 / f2;
    (* double := float;
    int := ENTIER(float);
    Out.Real(float); Out.Ln; *)
    Out.Int(i1); Out.Ln
END Floats.