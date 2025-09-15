MODULE Mandelbrot;
IMPORT Out, SYSTEM;

TYPE INT32 = INTEGER;
     F64 = LONGREAL;

PROCEDURE DoMandelbrot(size: INT32): INT32;
  VAR
    sum, byteAcc, bitNum: INT32;
    y, x, z: INT32;
    ci, cr: F64;
    zr, zi, zrzr, zizi: F64;
    notDone: BOOLEAN;
    escape: INT32;
BEGIN
  sum := 0; byteAcc := 0; bitNum := 0;

  y := 0;
  WHILE y < size DO
    ci := (2.0E64 *1.0E-64 * y / size) - 1.0;
    x := 0;
    WHILE x < size DO
      zr := 0.0; zrzr := 0.0;
      zi := 0.0; zizi := 0.0;
      cr := (2.0E64 *1.0E-64 * x / size) - 1.5;

      z := 0; notDone := TRUE; escape := 0;
      WHILE notDone & (z < 50) DO
        zr := zrzr - zizi + cr;
        zi := 2.0 * zr * zi + ci;

        zrzr := zr * zr;
        zizi := zi * zi;

        IF (zrzr + zizi) > 4.0 THEN
          notDone := FALSE;
          escape := 1
        END;
        INC(z)
      END;

      (* Out.String("> "); Out.Int(byteAcc, 0); Out.String(" | "); Out.Int(LSL(byteAcc, 1), 0);
      Out.String(" | "); Out.Int(escape, 0); Out.Ln; *)

      byteAcc := (* SYSTEM.LSH *) LSL(byteAcc, 1) + escape; (* byte_acc << 1 *)
      INC(bitNum);
      IF bitNum = 8 THEN
        sum := SYSTEM.VAL(INT32, SYSTEM.VAL(SET, sum) / SYSTEM.VAL(SET, byteAcc)); (* sum XOR byte_acc *)
        byteAcc := 0; bitNum := 0
      ELSIF x = size - 1 THEN
        byteAcc := (* SYSTEM.LSH *) LSL(byteAcc, 8 - bitNum);
        sum := SYSTEM.VAL(INT32, SYSTEM.VAL(SET, sum) / SYSTEM.VAL(SET, byteAcc)); (* sum XOR byte_acc *)
        byteAcc := 0; bitNum := 0
      END;
      INC(x)
    END;
    Out.Int(sum, 0); Out.Ln;
    INC(y)
  END;
  Out.Ln; Out.Int(sum, 0); Out.Ln;
  RETURN sum
END DoMandelbrot;

BEGIN
  DoMandelbrot(50)
END Mandelbrot.