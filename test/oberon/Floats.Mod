MODULE Floats;
IMPORT Out, Math;

CONST MAX_INTEGER = 2147483647;
      MIN_INTEGER = -2147483648;
      MAX_REAL = 3.40282E+38;
      MIN_REAL = 1.17549E-38;

VAR f1, f2: REAL;
    d1, d2: LONGREAL;
    i1, i2: INTEGER;
    l1, l2: LONGINT;

BEGIN
    i1 := 1; Out.Int(i1); Out.Ln;
    i2 := 2; Out.Int(i2); Out.Ln;
    l1 := i1; Out.Long(l1); Out.Ln;
    d1 := f1;
    f1 := Math.pi; Out.Real(f1); Out.Ln;
    f1 := i1; Out.Real(f1); Out.Ln;
    f2 := i1 + i2; Out.Real(f2); Out.Ln;
    f2 := i1 - f2; Out.Real(f2); Out.Ln;
    f2 := f1 * i2; Out.Real(f2); Out.Ln;
    f2 := f1 / f2; Out.Real(f2); Out.Ln;
    Out.Real(Math.sqrt(2)); Out.Ln;
    Out.Real(Math.exp(12.0)); Out.Ln;
    Out.Real(Math.ln(Math.e)); Out.Ln;
    Out.Real(Math.sin(Math.pi)); Out.Ln;
    Out.Real(Math.cos(Math.pi)); Out.Ln;
    Out.Real(Math.arctan(Math.pi)); Out.Ln;
    Out.Int(Math.entier(Math.pi)); Out.Ln;
    Out.Real(Math.real(3)); Out.Ln;
    f1 := 0.1;
    f2 := 0.2;
    Out.Real(f1 + f2); Out.Ln;
    Out.Real(9 / 5); Out.Ln
END Floats.