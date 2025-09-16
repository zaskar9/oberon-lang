MODULE Mandelbrot;
IMPORT Out, SYSTEM, Files, M := MathL;

CONST Size = 5000;
      Name = "mandelbrot.pgm";
      Max = 255;  (* maximum number of iterations *)
      EscapeRadius = 2.0;

VAR f: Files.File;
    r: Files.Rider;
    res: INTEGER;
    pos: LONGINT;
    str: ARRAY 32 OF CHAR;

PROCEDURE WriteNum(VAR r: Files.Rider; x: INTEGER);
VAR i: INTEGER;
    a: ARRAY 20 OF CHAR;
BEGIN
  ASSERT(x >= 0);
  ASSERT(LEN(str) > 20);
  i := 0;
  REPEAT
    a[i] := CHR(SHORT(x MOD 10) + 30H); x := x DIV 10; INC(i)
  UNTIL x = 0;
  REPEAT DEC(i); Files.WriteChar(r, a[i]) UNTIL i = 0
END WriteNum;

PROCEDURE DoMandelbrot*(VAR r: Files.Rider; size: INTEGER);
  VAR
    x, y, count, color: INTEGER;
    zx, zy, zx2, zy2, cx, cy, mag, nu: LONGREAL;
    scale: REAL;

  BEGIN
    scale := 3.0 / size;  (* maps pixel coordinates to [-2..1] × [-1.5..1.5] *)

    FOR y := 0 TO size - 1 DO
      cy := (y - size / 2.0) * scale;
      FOR x := 0 TO size - 1 DO
        cx := (x - size / 2.0) * scale;

        zx := 0.0; zy := 0.0;
        zx2 := 0.0; zy2 := 0.0;

        count := 0;
        WHILE (zx2 + zy2 <= EscapeRadius * EscapeRadius) & (count < Max) DO
          zy := 2.0 * zx * zy + cy;
          zx := zx2 - zy2 + cx;
          zx2 := zx * zx;
          zy2 := zy * zy;
          INC(count)
        END;

        IF count = Max THEN
          color := 0
        ELSE
          mag := M.sqrt(zx2 + zy2);
          nu := count + 1 - M.ln(M.ln(mag)) / M.ln(2.0);
          color := SHORT(ENTIER(255.0 * M.sqrt(nu / Max)));
          IF color > 255 THEN
            color := 255
          ELSIF color < 1 THEN
            color := 1
          END;
          color := color
        END;
        WriteNum(r, color); Files.WriteChar(r, 20X)
      END
    END
  END DoMandelbrot;

BEGIN
  f := Files.Old(Name);
  res := 0;
  IF (f = NIL) THEN
      f := Files.New(Name);
      Files.Register(f);
      DISPOSE(f);
      f := Files.Old(Name)
  END;
  ASSERT(f # NIL);
  Files.Set(r, f, 0);
  Files.WriteChar(r, "P");
  Files.WriteChar(r, "2");
  Files.WriteChar(r, 0AX);
  WriteNum(r, Size); Files.WriteChar(r, 20X);
  WriteNum(r, Size); Files.WriteChar(r, 0AX);
  WriteNum(r, 255); Files.WriteChar(r, 0AX);
  DoMandelbrot(r, Size);
  Files.Close(f);
  DISPOSE(f)
END Mandelbrot.