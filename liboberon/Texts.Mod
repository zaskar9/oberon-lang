(*JG 21.11.90 / NW 11.7.90 / 24.12.95 / 22.11.10 / 18.11.2014 / 10.1.2019 / AP 15.9.20 / MG 28.3.2024 *)
MODULE Texts;
IMPORT SYSTEM, Reals;

CONST
    TAB = 9X; CR = 0DX; LF = 0AX;

TYPE
    Writer* = RECORD col*: INTEGER END;

VAR
    W: Writer;

  (* Import `putchar` function from C <stdio.h> library. *)
  PROCEDURE putchar(ch: CHAR): INTEGER; EXTERN;

  PROCEDURE Write* (VAR W: Writer; ch: CHAR);
  BEGIN
    putchar(ch)
  END Write;

  PROCEDURE WriteLn* (VAR W: Writer);
  BEGIN Write(W, LF)
  END WriteLn;

  PROCEDURE WriteString* (VAR W: Writer; s: ARRAY OF CHAR);
    VAR i: INTEGER;
  BEGIN
    i := 0;
    WHILE (i < LEN(s)) & (s[i] # "\0") DO Write(W, s[i]); INC(i) END
  END WriteString;

  PROCEDURE WriteInt* (VAR W: Writer; x, n: LONGINT);
    VAR i: INTEGER; x0: LONGINT;
        a: ARRAY 20 OF CHAR;
  BEGIN
    IF x = MIN(LONGINT) THEN
      WHILE n > 20 DO Write(W, " "); DEC(n) END;
      WriteString(W, "-9223372036854775808")
    ELSE
      i := 0;
      IF x < 0 THEN DEC(n); x0 := -x ELSE x0 := x END;
      REPEAT
        a[i] := CHR(SHORT(x0 MOD 10) + 30H); x0 := x0 DIV 10; INC(i)
      UNTIL x0 = 0;
      WHILE n > i DO Write(W, " "); DEC(n) END;
      IF x < 0 THEN Write(W, "-") END;
      REPEAT DEC(i); Write(W, a[i]) UNTIL i = 0
    END
  END WriteInt;

  PROCEDURE WriteHexHelper(VAR W: Writer; x: LONGINT; n: INTEGER);
    VAR i, d: INTEGER; x0: LONGINT;
        a: ARRAY 16 OF CHAR;
  BEGIN
    x0 := x;
    IF x < 0 THEN INC(x0); x0 := -x0 END;
    FOR i := 0 TO 15 DO
      d := SHORT(x0 MOD 10H);
      IF x < 0 THEN d := 15 - d END;
      IF d < 10 THEN a[i] := CHR(d + 30H) ELSE a[i] := CHR(d + 37H) END;
      x0 := x0 DIV 10H
    END;
    i := n;
    REPEAT DEC(i); Write(W, a[i]) UNTIL i = 0
  END WriteHexHelper;

  PROCEDURE WriteShortHex* (VAR W: Writer; x: SHORTINT);
  BEGIN
    WriteHexHelper(W, x, 2 * SHORT(SYSTEM.SIZE(SHORTINT)))
  END WriteShortHex;

  PROCEDURE WriteHex* (VAR W: Writer; x: INTEGER);
  BEGIN
    WriteHexHelper(W, x, 2 * SHORT(SYSTEM.SIZE(INTEGER)))
  END WriteHex;

  PROCEDURE WriteLongHex* (VAR W: Writer; x: LONGINT);
  BEGIN
    WriteHexHelper(W, x, 2 * SHORT(SYSTEM.SIZE(LONGINT)))
  END WriteLongHex;

  PROCEDURE WriteReal* (VAR W: Writer; x: REAL; n: INTEGER);
    CONST maxD = 9;
    VAR e: INTEGER;
        h: LONGINT;
        z: REAL;
        d: ARRAY maxD OF CHAR;
  BEGIN
    e := Reals.Expo(x);
    IF e = 0 THEN
      WHILE n > 1 DO Write(W, " "); DEC(n) END;
      WriteString(W, "0")
    ELSIF e = 255 THEN
      WHILE n > 4 DO Write(W, " "); DEC(n) END;
      h := Reals.NaNCode(x);
      IF h # 0 THEN
        WHILE n > 3 DO Write(W, " "); DEC(n) END;
        WriteString(W, "NaN")
      ELSIF x < 0 THEN WriteString(W, "-INF")
      ELSE
        WHILE n > 3 DO Write(W, " "); DEC(n) END;
        WriteString(W, "INF")
      END
    ELSE
      IF n <= maxD THEN n := 3 ELSE DEC(n, 6) END;
      WHILE n > maxD DO Write(W, " "); DEC(n) END;
      (* there are 2 <= n < maxD digits to be written *)
      IF x < 0.0 THEN Write(W, "-"); x := -x ELSE Write(W, " ") END;
      e := (e - 127) * 301 DIV 1000; (* ln(2)/ln(10) = 0.301029996 *)
      IF e >= 0 THEN x := x / Reals.Ten(e) ELSE x := Reals.Ten(-e) * x END;
      IF x >= 10.0 THEN x := 0.1 * x; INC(e) END;
      z := Reals.Ten(n - 1); x := z * x + 0.5;
      IF x >= 10.0 * z THEN x := x * 0.1; INC(e) END;
      Reals.Convert(x, n, d);
      DEC(n); Write(W, d[n]); Write(W, ".");
      REPEAT DEC(n); Write(W, d[n]) UNTIL n = 0;
      Write(W, "E");
      IF e < 0 THEN Write(W, "-"); e := -e ELSE Write(W, "+") END;
      Write(W, CHR(e DIV 10 + 30H));
      Write(W, CHR(e MOD 10 + 30H))
    END
  END WriteReal;

  PROCEDURE WriteLongReal* (VAR W: Writer; x: LONGREAL; n: INTEGER);
 		CONST maxD = 16;
    VAR e, i: INTEGER;
        h, l: LONGINT;
        z: LONGREAL;
        d: ARRAY maxD OF CHAR;
  BEGIN
    e := Reals.ExpoL(x);
    IF e = 0 THEN
      WHILE n > 1 DO Write(W, " "); DEC(n) END;
      WriteString(W, "0")
    ELSIF e = 2047 THEN
      WHILE n > 4 DO Write(W, " "); DEC(n) END;
      Reals.NaNCodeL(x, h, l);
      IF (h # 0) OR (l # 0) THEN
        WHILE n > 3 DO Write(W, " "); DEC(n) END;
        WriteString(W, "NaN")
      ELSIF x < 0 THEN WriteString(W, "-INF")
      ELSE
        WHILE n > 3 DO Write(W, " "); DEC(n) END;
        WriteString(W, "INF")
      END
    ELSE
      IF n <= 9 THEN n := 1 ELSE DEC(n, 8) END;
      WHILE n >= maxD DO Write(W, " "); DEC(n) END;
      IF (e # 0) & (x < 0) THEN Write(W, "-"); x := - x ELSE Write(W, " ") END;
      IF e = 0 THEN h := 0; l := 0 (* no denormals *)
      ELSE e := (e - 1023) * 301029 DIV 1000000; (* ln(2)/ln(10) = 0.301029996 *)
        z := Reals.TenL(e+1);
        IF x >= z THEN x := x / z; INC(e) ELSE x := x * Reals.TenL(-e) END;
        IF x >= 10 THEN x := x * Reals.TenL(-1) + 0.5E0 / Reals.TenL(n); INC(e)
        ELSE x := x + 0.5E0 / Reals.TenL(n);
          IF x >= 10 THEN x := x * Reals.TenL(-1); INC(e) END
        END;
        x := x * Reals.TenL(7); h := ENTIER(x); x := (x - h) * Reals.TenL(8); l := ENTIER(x)
      END;
      i := maxD - 1;
      WHILE i > 7 DO d[i]:= CHR(SHORT(l MOD 10) + 30H); l := l DIV 10; DEC(i) END;
      WHILE i >= 0 DO d[i]:= CHR(SHORT(h MOD 10) + 30H); h := h DIV 10; DEC(i) END;
      Write(W, d[0]); Write(W, "."); i := 1; WHILE i <= n DO Write(W, d[i]); INC(i) END;
 			Write(W, "E");
      IF e < 0 THEN Write(W, "-"); e := - e ELSE Write(W, "+") END;
      Write(W, CHR(e DIV 100 + 30H)); e := e MOD 100;
      Write(W, CHR(e DIV 10 + 30H));
      Write(W, CHR(e MOD 10 + 30H))
    END
  END WriteLongReal;

END Texts.