MODULE Types;

IMPORT Strings;

CONST BUFSIZE* = 256;

(* IsIntDigit checks if an INTEGER is in the range of 0 through 9 *)
PROCEDURE IsIntDigit*(i : INTEGER) : BOOLEAN;
BEGIN
  RETURN ((i >= 0) & (i <= 9))
END IsIntDigit;

(* IsCharDigit checks ch and returns TRUE if a digit ("0" .. "9" found)
   or FALSE if it is not a digit. *)
PROCEDURE IsCharDigit*(ch : CHAR) : BOOLEAN;
BEGIN
  RETURN ((ch >= "0") & (ch <= "9"))
END IsCharDigit;

(* CharToDigit converts an ASCII char representing an single digit to
   an integer value. *)
PROCEDURE CharToDigit*(ch : CHAR; VAR ok : BOOLEAN) : INTEGER;
BEGIN
  ok := IsCharDigit(ch);
  RETURN (ORD(ch) - ORD("0"))
END CharToDigit;

(* DigitToChar converts an INTEGER in range of 0 through 9 to a character *)
PROCEDURE DigitToChar*(i : INTEGER; VAR ok : BOOLEAN) : CHAR;
BEGIN
  ok := IsIntDigit(i);
  RETURN (CHR(ORD("0") + i))
END DigitToChar;

(* IntShiftRight converts the input integer to a real, multiplies by 0.1
   and converts by to an integer. The value in the ones column is record
   in the VAR parameter r.  E.g. IntShiftRight(123) return 12, r is set to 3. *)
PROCEDURE IntShiftRight*(x : INTEGER; VAR r : INTEGER) : INTEGER;
  VAR i : INTEGER; isNeg : BOOLEAN;
BEGIN
  isNeg := (x < 0);
  i := FLOOR(FLT(ABS(x)) * 0.1);
  r := ABS(x) - (i * 10);
  IF isNeg THEN
    i := i * (-1);
  END;
  RETURN i
END IntShiftRight;

(* IntShiftLeft multiples input value by 10 and adds y. E.g. IntShiftLeft(123, 4) return 1234 *)
PROCEDURE IntShiftLeft*(x, y : INTEGER) : INTEGER;
  VAR i : INTEGER; isNeg : BOOLEAN;
BEGIN
  isNeg := (x < 0);
  i := (ABS(x) * 10) + y;
  IF isNeg THEN
    i := i * (-1);
  END;
  RETURN i
END IntShiftLeft;

(* Itoa converts an INTEGER to an ASCII string setting ok BOOLEAN to
   TRUE if value ARRAY OF CHAR holds the full integer, FALSE if
   value was too small to hold the integer value.  *)
PROCEDURE Itoa*(x : INTEGER; VAR value : ARRAY OF CHAR; ok : BOOLEAN);
  VAR i, j, k, l, minL : INTEGER; tmp : ARRAY BUFSIZE OF CHAR; isNeg : BOOLEAN;
BEGIN
  i := 0; j := 0; k := 0; l := LEN(value); isNeg := (x < 0);
  IF isNeg THEN
    (* minimum string length for value is 3, negative sign, digit and 0X *)
    minL := 3;
  ELSE 
    (* minimum string length for value is 2, one digit and 0X *)
    minL := 2; 
  END;
  ok := (l >= minL) & (LEN(value) >= LEN(tmp));
  IF ok THEN
    IF IsIntDigit(ABS(x)) THEN
      IF isNeg THEN
         value[i] := "-"; INC(i);
      END;
      value[i] := DigitToChar(ABS(x), ok); INC(i); value[i] := 0X;
    ELSE
      x := ABS(x); (* We need to work with the absolute value of x *)
      i := 0; tmp[i] := 0X;
      WHILE (x >= 10) & ok DO
        (* extract the ones columns *)
        x := IntShiftRight(x, k); (* a holds the shifted value, "k" holds the ones column value shifted out. *)
        (* write append k to our temp array holding values in reverse number magnitude *)
        tmp[i] := DigitToChar(k, ok); INC(i); tmp[i] := 0X;
      END;
      (* We now can convert the remaining "ones" column. *)
      tmp[i] := DigitToChar(x, ok); INC(i); tmp[i] := 0X;
      IF ok THEN
        (* now reverse the order of tmp string append each character to value *)
        i := 0; j := Strings.Length(tmp) - 2;
        IF isNeg THEN
          value[i] := "-"; INC(i);
        END;
        j := Strings.Length(tmp) - 1;
        WHILE (j > -1) DO
          value[i]:= tmp[j]; 
          INC(i); DEC(j);
          value[i] := 0X;
        END;
        value[i] := 0X;
      END;
    END; 
  ELSE
    ok := FALSE;
  END;
END Itoa;

(* magnitude takes x and multiplies it be 10^y, If y is positive zeros
   are appended to the right side (i.e. multiplied by 10). If y is negative
   then the result is shifted left (i.e. multiplied by 0.1 via 
   IntShiftRight().).  The digit(s) shift to the fractional
   side of the decimal are ignored. *)
PROCEDURE magnitude(x, y : INTEGER) : INTEGER;
  VAR z, w : INTEGER;
BEGIN
  z := 1;
  IF y >= 0 THEN
    WHILE y > 0 DO
      z := IntShiftLeft(z, 0);
      DEC(y);
    END;
  ELSE
    WHILE y < 0 DO
      x := IntShiftRight(x, w);
      INC(y);
    END;
  END;
  RETURN (x * z)
END magnitude;

(* Atoi converts an ASCII string to a signed integer value
   setting the ok BOOLEAN to TRUE on success and FALSE on error. *)
PROCEDURE Atoi*(source : ARRAY OF CHAR; VAR value : INTEGER; VAR ok : BOOLEAN);
  VAR i, l, a, m: INTEGER; isNeg : BOOLEAN;
BEGIN
  (* "i" is the current CHAR position we're analyzing, "l" is the
     length of our string, "a" holds the accumulated value,
     "m" holds the current magnitude we're working with *)
  i := 0; l := Strings.Length(source);
  a := 0; m := l - 1; isNeg := FALSE; ok := TRUE;
  (* Validate magnitude and sign behavior *)
  IF (l > 0) & (source[0] = "-") THEN
    INC(i); DEC(m);
    isNeg := TRUE;
  ELSIF (l > 0) & (source[0] = "+") THEN
    INC(i); DEC(m);
  END;

  (* The accumulator should always hold a positive integer, if the
     sign flips we have overflow, ok should be set to FALSE *)
  ok := TRUE;
  WHILE (i < l) & ok DO
    a := a + magnitude(CharToDigit(source[i], ok), m);
    IF a < 0 THEN
      ok := FALSE; (* we have an overflow condition *)
    END;
    DEC(m);
    INC(i);
  END;
  IF ok THEN
    IF (i = l) THEN
      IF isNeg THEN
        value := a * (-1);
      ELSE
        value := a;
      END;
    END;
  END;
END Atoi;

END Types.
