(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 * This module is based on the Oberon A2 version of Texts.Mod.
 *)

MODULE Texts; (*JG 21.11.90 / NW 11.7.90 / 24.12.95 / 22.11.10 / 18.11.2014 / 10.1.2019 / AP 15.9.20 / MG 28.3.2024 *)
IMPORT SYSTEM, Reals;

CONST
    TAB = 9X; CR = 0DX; LF = 0AX;

TYPE
    Writer* = RECORD col*: INTEGER END;

VAR
    W: Writer;


  (* Declare `int putchar(char)` from C <stdio.h> library. *)
  PROCEDURE [ "C" ] PutChar(ch: CHAR): INTEGER; EXTERNAL [ "putchar" ];


  PROCEDURE Write* (VAR W: Writer; ch: CHAR);
  BEGIN
    PutChar(ch)
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
    VAR i, d: INTEGER;
        a: ARRAY 16 OF CHAR;
  BEGIN
    FOR i := 0 TO 15 DO
      d := SHORT(x MOD 10H);
      IF d < 10 THEN a[i] := CHR(d + 30H) ELSE a[i] := CHR(d + 37H) END;
      x := x DIV 10H
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

  (** Write REAL value x to W's buffer using n character positions. *)
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

  (** Write REAL value x in a fixed point notation, where  n is the overall minimal length for the output field,
      f the number of fraction digits following the decimal point, and E the fixed exponent (printed iff E # 0). *)
  PROCEDURE WriteRealFix* (VAR W: Writer; x: REAL; n, f, E: INTEGER);
    (* BM 1993.4.22. Do not simplify rounding! / JG formatting adjusted *)
    VAR e, i: INTEGER;
        h: LONGINT;
        r, y: LONGREAL;
        z: REAL;
        s: CHAR;
        d: ARRAY 8 OF CHAR;
  BEGIN
    e := Reals.Expo(x);
    IF (e = 255) OR (ABS(E) > 38) THEN
      WHILE n > 8 DO Write(W, " "); DEC(n) END;
      h := Reals.NaNCode(x);
      IF h # 0 THEN WriteString(W, "     NaN")
      ELSIF x < 0 THEN WriteString(W, "    -INF")
      ELSE WriteString(W, "      INF")
      END
    ELSE
      IF E = 0 THEN DEC(n, 2) ELSE DEC(n, 6) END;
      IF f < 0 THEN f := 0 END;
      IF n < f + 2 THEN n := f + 2 END;
      DEC(n, f);
      IF (e # 0) & (x < 0) THEN s:= "-"; x:= - x ELSE s:= " " END;
      IF e = 0 THEN h := 0; DEC(e, E-1) (* no denormals *)
      ELSE
        e := (e - 127) * 301 DIV 1000; (* ln(2)/ln(10) = 0.301029996 *)
        IF e < 38 THEN z := Reals.Ten(e+1);
          IF x >= z THEN y := LONG(x)/LONG(z); INC(e) ELSE y := x * Reals.Ten(-e) END
        ELSE y := x * Reals.Ten(-38) END;
        DEC(e, E-1); i := -(e+f);
        IF i <= 0 THEN r := 5 * Reals.Ten(i) ELSE r := 0 END;
        IF y >= 10 THEN y := y * Reals.Ten(-1) + r; INC(e)
        ELSE y := y + r;
          IF y >= 10 THEN y := y * Reals.Ten(-1); INC(e) END
        END;
        y := y * Reals.Ten(7); h := ENTIER(y)
      END;
      i := 7;
      WHILE i >= 0 DO d[i] := CHR(h MOD 10 + ORD("0")); h := h DIV 10; DEC(i) END;
      IF n <= e THEN n := e + 1 END;
      IF e > 0 THEN WHILE n > e DO Write(W, " "); DEC(n) END;
        Write(W, s); e := 0;
        WHILE n > 0 DO DEC(n);
          IF e < 8 THEN Write(W, d[e]); INC(e) ELSE Write(W, "0") END
        END;
        Write(W, ".")
      ELSE
        WHILE n > 1 DO Write(W, " "); DEC(n) END;
        Write(W, s); Write(W, "0"); Write(W, ".");
        WHILE (0 < f) & (e < 0) DO Write(W, "0"); DEC(f); INC(e) END
      END;
      WHILE f > 0 DO DEC(f);
        IF e < 8 THEN Write(W, d[e]); INC(e) ELSE Write(W, "0") END
      END;
      IF E # 0 THEN
        IF E < 0 THEN WriteString(W, "E-"); E := - E ELSE WriteString(W, "E+") END;
        Write(W, CHR(E DIV 10 + 30H));
        Write(W, CHR(E MOD 10 + 30H))
      END
    END
  END WriteRealFix;

  (** Write LONGREAL value x to W's buffer using n character positions. *)
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
        IF x >= 10 THEN x := x * Reals.TenL(-1) + 0.5D0 / Reals.TenL(n); INC(e)
        ELSE x := x + 0.5D0 / Reals.TenL(n);
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

  (** Write LONGREAL value x in a fixed point notation, where n is the overall minimal length for the output field,
      f the number of fraction digits following the decimal point, and D the fixed exponent (printed iff D # 0). *)
  PROCEDURE WriteLongRealFix* (VAR W: Writer; x: LONGREAL; n, f, D: INTEGER);
    (* BM 1993.4.22. Do not simplify rounding! / JG formatting adjusted *)
    CONST maxD = 16;
    VAR e, i: INTEGER;
        h, l: LONGINT;
        r, z: LONGREAL;
        d: ARRAY maxD OF CHAR;
        s: CHAR;
  BEGIN
    e := Reals.ExpoL(x);
    IF (e = 2047) OR (ABS(D) > 308) THEN
      WHILE n > 9 DO Write(W, " "); DEC(n) END;
      Reals.NaNCodeL(x, h, l);
      IF (h # 0) OR (l # 0) THEN WriteString(W, "      NaN")
      ELSIF x < 0 THEN WriteString(W, "     -INF")
      ELSE WriteString(W, "      INF")
      END
    ELSE
      IF D = 0 THEN DEC(n, 2) ELSE DEC(n, 7) END;
      IF n < 2 THEN n := 2 END;
      IF f < 0 THEN f := 0 END;
      IF n < f + 2 THEN n := f + 2 END;
      DEC(n, f);
      IF (e # 0) & (x < 0) THEN s := "-"; x := - x ELSE s := " " END;
      IF e = 0 THEN h := 0; l := 0; DEC(e, D-1) (* no denormals *)
      ELSE
        e := (e - 1023) * 301029 DIV 1000000; (* ln(2)/ln(10) = 0.301029996 *)
        z := Reals.Ten(e+1);
        IF x >= z THEN x := x/z; INC(e) ELSE x:= x * Reals.Ten(-e) END;
        DEC(e, D-1); i := -(e+f);
        IF i <= 0 THEN r := 5 * Reals.Ten(i) ELSE r := 0 END;
        IF x >= 10 THEN x := x * Reals.Ten(-1) + r; INC(e)
        ELSE x := x + r;
          IF x >= 10 THEN x := x * Reals.Ten(-1); INC(e) END
        END;
        x := x * Reals.Ten(7); h:= ENTIER(x); x := (x-h) * Reals.Ten(8); l := ENTIER(x)
      END;
      i := 15;
      WHILE i > 7 DO d[i] := CHR(l MOD 10 + ORD("0")); l := l DIV 10; DEC(i) END;
      WHILE i >= 0 DO d[i] := CHR(h MOD 10 + ORD("0")); h := h DIV 10; DEC(i) END;
      IF n <= e THEN n := e + 1 END;
      IF e > 0 THEN WHILE n > e DO Write(W, " "); DEC(n) END;
        Write(W, s); e:= 0;
        WHILE n > 0 DO DEC(n);
          IF e < maxD THEN Write(W, d[e]); INC(e) ELSE Write(W, "0") END
        END;
        Write(W, ".")
      ELSE
        WHILE n > 1 DO Write(W, " "); DEC(n) END;
        Write(W, s); Write(W, "0"); Write(W, ".");
        WHILE (0 < f) & (e < 0) DO Write(W, "0"); DEC(f); INC(e) END
      END;
      WHILE f > 0 DO DEC(f);
        IF e < maxD THEN Write(W, d[e]); INC(e) ELSE Write(W, "0") END
      END;
      IF D # 0 THEN
        IF D < 0 THEN WriteString(W, "D-"); D := - D ELSE WriteString(W, "D+") END;
        Write(W, CHR(D DIV 100 + 30H)); D := D MOD 100;
        Write(W, CHR(D DIV 10 + 30H));
        Write(W, CHR(D MOD 10 + 30H))
      END
    END
  END WriteLongRealFix;

END Texts.