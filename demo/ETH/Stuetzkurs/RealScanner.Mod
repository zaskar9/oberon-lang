MODULE RealScanner;
IMPORT In, Out, Reals;

  TYPE
    String = ARRAY 32 OF CHAR;
  VAR
    str: String;

  PROCEDURE ctoi(ch: CHAR): INTEGER;
  BEGIN
    RETURN ORD(ch) - ORD("0")
  END ctoi;

  PROCEDURE isDigit(ch: CHAR): BOOLEAN;
  BEGIN
    RETURN (ch >= "0") & (ch <= "9")
  END isDigit;

  PROCEDURE Scan(str: String): REAL;
  VAR
    res: REAL;
    i, s, scale: INTEGER;
    val, div: LONGINT;
    ch: CHAR;
    valid: BOOLEAN;
  BEGIN
    val := 0; i := 0; s := 0; div := 1; scale := 0 valid := TRUE;
    WHILE valid & (i < LEN(str)) & (str[i] # 0X) DO
      ch := str[i]; INC(i);
      CASE s OF
        0 : IF isDigit(ch) THEN
              s := 1
              val := 10 * val + ctoi(ch)
            ELSE
              valid := FALSE
            END
      | 1 : IF isDigit(ch) THEN
              val := 10 * val + ctoi(ch)
            ELSIF ch = "." THEN
              s := 2
            ELSE
              valid := FALSE
            END
      | 2 : IF isDigit(ch) THEN
              val := 10 * val + ctoi(ch);
              div := div * 10
            ELSIF (ch = "E") OR (ch = "D") THEN
              s := 3
            ELSE
              valid := FALSE
            END
      | 3 : IF isDigit(ch) THEN
              scale := ctoi(ch);
              s := 5
            ELSIF ch = "+" THEN
              scale := 1;
              s := 4
            ELSIF ch = "-" THEN
              scale := -1;
              s := 4
            ELSE
              valid := FALSE
            END
      | 4 : IF isDigit(ch) THEN
              scale := scale * ctoi(ch);
              s := 5
            ELSE
              valid := FALSE
            END
      | 5 : IF isDigit(ch) THEN
              scale := 10 * scale + ctoi(ch)
            ELSE
              valid := FALSE
            END
      ELSE
        valid := FALSE
      END
    END;
    IF valid THEN
      res := val / div * Reals.Ten(scale)
    ELSE
      res := Reals.NaN()
    END;
    RETURN res
  END Scan;

BEGIN
  In.Open;
  In.String(str);
  Out.Real(Scan(str), 15); Out.Ln
END RealScanner.