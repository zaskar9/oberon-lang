MODULE In;

  VAR Done*: BOOLEAN;  (** Done indicates if the operation was successful. *)


  (* Declare `bool olang_in_getchar( char * )` function from Oberon runtime library. *)
  PROCEDURE [ "C" ] getChar(VAR ch: CHAR): BOOLEAN; EXTERNAL [ "olang_in_getchar" ];

  (* Declare `bool olang_in_ungetchar( int )` function from Oberon runtime library. *)
  PROCEDURE [ "C" ] ungetChar(ch: CHAR): BOOLEAN; EXTERNAL [ "olang_in_ungetchar" ];

  (* Declare `bool olang_in_getfloat( float * )` function from Oberon runtime library. *)
  PROCEDURE [ "C" ] getFloat(VAR f: REAL): BOOLEAN; EXTERNAL [ "olang_in_getfloat" ];

  (* Declare `bool olang_in_getfloat( float * )` function from Oberon runtime library. *)
  PROCEDURE [ "C" ] getDouble(VAR d: LONGREAL): BOOLEAN; EXTERNAL [ "olang_in_getdouble" ];

  (* Declare `bool olang_in_getint( char *, int32_t *, bool )` function from Oberon runtime library. *)
  PROCEDURE [ "C" ] getInt(buf: ARRAY OF CHAR; VAR i: INTEGER; isHex: BOOLEAN): BOOLEAN; EXTERNAL [ "olang_in_getint" ];

  (* Declare `bool olang_in_getint( char *, int32_t *, bool )` function from Oberon runtime library. *)
  PROCEDURE [ "C" ] getLong(buf: ARRAY OF CHAR; VAR l: LONGINT; isHex: BOOLEAN): BOOLEAN; EXTERNAL [ "olang_in_getlong" ];


  PROCEDURE Open*;
  BEGIN
    Done := TRUE
  END Open;

  (** Returns the character `ch` at the current position. *)
  PROCEDURE Char*(VAR ch: CHAR);
  BEGIN
    Done := getChar(ch)
  END Char;

  PROCEDURE isSpace(ch: CHAR): BOOLEAN;
  BEGIN
    RETURN (ch = 20X) OR ((ch >= 0AX) & (ch <= 0DX)) OR (ch = 9X)
  END isSpace;

  PROCEDURE isGraph(ch: CHAR): BOOLEAN;
  BEGIN
    RETURN (ch >= 21X) & (ch <= 7EX)
  END isGraph;

  PROCEDURE isDigit(ch: CHAR): BOOLEAN;
  BEGIN
    RETURN (ch >= "0") & (ch <= "9")
  END isDigit;

  PROCEDURE isHexDigit(ch: CHAR): BOOLEAN;
  BEGIN
    RETURN isDigit(ch) OR ((ch >= "A") & (ch <= "F")) OR ((ch >= "a") & (ch <= "f"))
  END isHexDigit;

  PROCEDURE ScanInteger(VAR buf: ARRAY OF CHAR; VAR isHex: BOOLEAN): BOOLEAN;
  VAR ch: CHAR;
      i: INTEGER;
  BEGIN
    REPEAT
      Done := getChar(ch)
    UNTIL ~Done OR ~isSpace(ch);
    isHex := FALSE;
    i := 0;
    IF Done & (ch = "-") THEN buf[i] := ch; Done := getChar(ch); INC(i) END;
    IF Done & isDigit(ch) THEN
      WHILE Done & (i < LEN(buf)) & isHexDigit(ch) DO
        buf[i] := ch; INC(i);
        Done := getChar(ch)
      END;
      IF Done & (i < LEN(buf)) THEN
        buf[i] := 0X;
        IF (ch = "H") OR (ch = "h") THEN isHex := TRUE ELSE ungetChar(ch) END;
        RETURN TRUE
      END
    END;
    RETURN FALSE
  END ScanInteger;

  (** Returns the integer constant `n` at the current position according to the format:
        Integer = digit { digit } | digit { hexDigit } "H" .
      The operation requires `Done = TRUE` and guarantees (`Done = TRUE` and the result is valid) or (`Done = FALSE`).
      The operation skips leading blanks, tabs or end-of-line characters. *)
  PROCEDURE Int*(VAR n: INTEGER);
  VAR buf: ARRAY 12 OF CHAR;
      isHex: BOOLEAN;
  BEGIN
    ASSERT(Done);
    IF ScanInteger(buf, isHex) THEN
      Done := getInt(buf, n, isHex)
    ELSE
      Done := FALSE
    END
  END Int;

  (** Returns the long integer constant `n` at the current position according to the format:
        Integer = digit { digit } | digit { hexDigit } "H" .
      The operation requires `Done = TRUE` and guarantees (`Done = TRUE` and the result is valid) or (`Done = FALSE`).
      The operation skips leading blanks, tabs or end-of-line characters. *)
  PROCEDURE LongInt*(VAR n: LONGINT);
  VAR buf: ARRAY 21 OF CHAR;
      isHex: BOOLEAN;
  BEGIN
    ASSERT(Done);
    IF ScanInteger(buf, isHex) THEN
      Done := getLong(buf, n, isHex)
    ELSE
      Done := FALSE
    END
  END LongInt;

  (** Returns the real constant `x` at the current position according to the format:
        Real = digit { digit } [ { digit } [ "E" ( "+" | "-" ) digit { digit } ] ] .
      The operation requires `Done = TRUE` and guarantees (`Done = TRUE` and the result is valid) or (`Done = FALSE`).
      The operation skips leading blanks, tabs or end-of-line characters. *)
  PROCEDURE Real*(VAR x: REAL);
  BEGIN
    ASSERT(Done);
    Done := getFloat(x)
  END Real;

  (** Returns the long real constant `x` at the current position according to the format:
        Real = digit { digit } [ { digit } [ "E" ( "+" | "-" ) digit { digit } ] ] .
      The operation requires `Done = TRUE` and guarantees (`Done = TRUE` and the result is valid) or (`Done = FALSE`).
      The operation skips leading blanks, tabs or end-of-line characters. *)
  PROCEDURE LongReal*(VAR x: LONGREAL);
  BEGIN
    ASSERT(Done);
    Done := getDouble(x)
  END LongReal;

  (** Returns the string `str` at the current position according to the format:
        String = '"' char { char } '"' .
      The string must not contain characters less than blank such as EOL or TAB. The operation requires `Done = TRUE`
      and guarantees (`Done = TRUE` and the result is valid) or (`Done = FALSE`). The operation skips leading blanks,
      tabs or end-of-line characters. *)
  PROCEDURE String*(VAR str: ARRAY OF CHAR);
  VAR ch: CHAR;
      i: INTEGER;
  BEGIN
    ASSERT(Done);
    REPEAT
      Done := getChar(ch)
    UNTIL ~Done OR ~isSpace(ch);
    IF Done & (ch = 22X) THEN
      i := 0;
      Done := getChar(ch);
      WHILE Done & (i < LEN(str)) & (ch >= 20X) & (ch # 22X) DO
        str[i] := ch;
        Done := getChar(ch); INC(i)
      END;
      IF ch = 22X THEN str[i] := 0X ELSE Done := FALSE END
    ELSE
      Done := FALSE
    END
  END String;

  (** Returns the name `name` at the current position according to the file name format of the underlying operating
      system (e.g. "lib/My.Mod" under Unix). The operation requires `Done = TRUE` and guarantees (`Done = TRUE` and
      the result is valid) or (`Done = FALSE`). The operation skips leading blanks, tabs or end-of-line characters. *)
  PROCEDURE Name*(VAR name: ARRAY OF CHAR);
  VAR ch: CHAR;
      i: INTEGER;
  BEGIN
    ASSERT(Done);
    REPEAT
      Done := getChar(ch)
    UNTIL ~Done OR ~isSpace(ch);
    IF Done THEN
      i := 0;
      WHILE Done & (i < LEN(name)) & (isGraph(ch) OR (ch >= 80X)) DO
        name[i] := ch;
        Done := getChar(ch); INC(i)
      END;
      IF i < LEN(name) THEN name[i] := 0X ELSE Done := FALSE END
    END
  END Name;

  (** Returns the name `name` at the current position. The operation either reads until a newline character is found
      or the input is exhausted.  The operation requires `Done = TRUE` and guarantees (`Done = TRUE` and the result is
      valid) or (`Done = FALSE`). The operation skips leading blanks, tabs or end-of-line characters. *)
  PROCEDURE Line*(VAR line: ARRAY OF CHAR);
  VAR ch: CHAR;
      i: INTEGER;
  BEGIN
    ASSERT(Done);
    i := 0;
    Done := getChar(ch);
    WHILE Done & (ch # 0AX) DO
      IF i < LEN(line) THEN line[i] := ch END;
      Done := getChar(ch); INC(i)
    END;
    IF (i > 0) & (ch = 0AX) THEN
      IF i < LEN(line) THEN
        line[i] := 0X; Done := TRUE
      ELSE
        line[LEN(line) - 1] := 0X; Done := FALSE
      END
    ELSE
      Done := FALSE
    END
  END Line;

BEGIN
    Done := FALSE
END In.