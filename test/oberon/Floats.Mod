MODULE Floats;
IMPORT Out, Math;

CONST MAXINTEGER = 2147483647;
      MININTEGER = -2147483648;
      MAXREAL = 3.40282347E+38;
      MINREAL = 1.17549435E-38;


VAR f1, f2: REAL;
    d1, d2: LONGREAL;
    i1, i2: INTEGER;
    l1, l2: LONGINT;

BEGIN
    i1 := 1; Out.Int(i1, 0); Out.Ln;
    i2 := 2; Out.Int(i2, 0); Out.Ln;
    l1 := i1; Out.Long(l1, 0); Out.Ln;
    d1 := f1;
    f1 := Math.pi; Out.Real(f1, 16); Out.Ln;
    f1 := i1; Out.Real(f1, 16); Out.Ln;
    f2 := i1 + i2; Out.Real(f2, 16); Out.Ln;
    f2 := i1 - f2; Out.Real(f2, 16); Out.Ln;
    f2 := f1 * i2; Out.Real(f2, 16); Out.Ln;
    f2 := f1 / f2; Out.Real(f2, 16); Out.Ln;
    Out.Real(Math.sqrt(2), 16); Out.Ln;
    Out.Real(Math.exp(12.0), 16); Out.Ln;
    Out.Real(Math.ln(Math.e), 16); Out.Ln;
    Out.Real(Math.sin(Math.pi), 16); Out.Ln;
    Out.Real(Math.cos(Math.pi), 16); Out.Ln;
    Out.Real(Math.arctan(Math.pi), 16); Out.Ln;
    Out.Int(Math.entier(Math.pi), 0); Out.Ln;
    Out.Real(Math.real(3), 16); Out.Ln;
    f1 := 0.1;
    f2 := 0.2;
    Out.Real(f1 + f2, 16); Out.Ln;
    Out.Real(9 / 5, 16); Out.Ln;
    Out.String("["); Out.Int(MININTEGER, 0); Out.String(", "); Out.Int(MAXINTEGER, 0); Out.String("]"); Out.Ln;
    Out.String("["); Out.Real(MINREAL, 0); Out.String(", "); Out.Real(MAXREAL, 0); Out.String("]"); Out.Ln
END Floats.