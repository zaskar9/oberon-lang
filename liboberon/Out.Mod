MODULE Out;

  IMPORT Texts;

  VAR
    W: Texts.Writer;

  (* Writes the character `ch` to the output. *)
  PROCEDURE Char* (ch: CHAR);
  BEGIN
    Texts.Write(W, ch)
  END Char;

  (* Writes the string `s` to the output. *)
  PROCEDURE String* (s: ARRAY OF CHAR);
  BEGIN
    Texts.WriteString(W, s)
  END String;

  (* Outputs a newline. *)
  PROCEDURE Ln*;
  BEGIN
    Texts.WriteLn(W)
  END Ln;

  (* Writes the integer `x` in `n` field positions. *)
  PROCEDURE Int* (x: INTEGER; n: INTEGER);
  BEGIN
    Texts.WriteInt(W, x, n)
  END Int;

  (* Writes the long integer `x` in `n` field positions. *)
  PROCEDURE Long* (x: LONGINT; n: INTEGER);
  BEGIN
    Texts.WriteInt(W, x, n)
  END Long;

  (* Writes the integer `x` in hexadecimal notation. *)
  PROCEDURE Hex* (x: INTEGER);
  BEGIN
    Texts.WriteHex(W, x)
  END Hex;

  (* Writes the long integer `x` in hexadecimal notation. *)
  PROCEDURE LongHex* (x: LONGINT);
  BEGIN
    Texts.WriteLongHex(W, x)
  END LongHex;

  (* Writes the real `x` in `n` field positions. *)
  PROCEDURE Real* (x: REAL; n: INTEGER);
  BEGIN
    Texts.WriteReal(W, x, n)
  END Real;

  (* Writes the long real `x` in `n` field positions. *)
  PROCEDURE LongReal* (x: LONGREAL; n: INTEGER);
  BEGIN
    Texts.WriteLongReal(W, x, n)
  END LongReal;

  (* Writes the set `set` to the output. *)
  PROCEDURE Set* (set: SET);
  VAR i: INTEGER;
  BEGIN
    Texts.WriteString(W, "{ ");
    FOR i := 0 TO 31 DO
      IF i IN set THEN
        Texts.WriteInt(W, i, 0); Texts.Write(W, " ")
      END
    END;
    Texts.Write(W, "}")
  END Set;

END Out.