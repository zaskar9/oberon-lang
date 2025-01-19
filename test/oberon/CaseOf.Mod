MODULE CaseOf;
IMPORT Out;

PROCEDURE CharClass(ch: CHAR);
BEGIN
  CASE ch OF
      " ": Out.String("whitespace character")
    |  0X: Out.String("string terminator")
    | ":", ";", ",", ".", "?", "!": Out.String("punctuation")
    | "A" .. "Z": Out.String("uppercase letter")
    | "a" .. "z": Out.String("lowercase letter")
    | "0" .. "9": Out.String("digit")
  ELSE
    Out.String("special character")
  END;
  Out.Ln
END CharClass;

PROCEDURE PrintCharClass(s: ARRAY OF CHAR);
VAR i: INTEGER;
    ch: CHAR;
BEGIN
    FOR i := 0 TO SHORT(LEN(s)) - 1 DO
        ch := s[i];
        Out.Char(ch);
        Out.String(":\t");
        CharClass(s[i])
    END
END PrintCharClass;

BEGIN
    PrintCharClass("John's birthday is: August 1st!\n")
END CaseOf.