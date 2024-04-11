MODULE Issue37;
IMPORT Out;

PROCEDURE Length(str: ARRAY OF CHAR) : INTEGER;
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE (i < LEN(str)) & (str[i] # "\0") DO INC(i) END;
    RETURN i
END Length;

PROCEDURE FillString(VAR s : ARRAY OF CHAR);
VAR
   i : INTEGER;
BEGIN
   i := 0;
   WHILE i < LEN(s) DO
      s[i] := CHR(SHORT(i MOD 20) + 65);
      INC(i)
   END
END FillString;

PROCEDURE Run1();
VAR
    str : ARRAY 257 OF CHAR;
BEGIN
    str := "test1";
    Out.String("1.1: Length(str) ="); Out.Int(Length(str), 5); Out.Long(LEN(str), 5); Out.Ln;
    FillString(str);
    Out.String("1.2: Length(str) ="); Out.Int(Length(str), 5); Out.Long(LEN(str), 5); Out.Ln
END Run1;

PROCEDURE Run2();
VAR
    str : ARRAY 257 OF CHAR;
    PROCEDURE Assert(b: BOOLEAN);
    BEGIN
        ASSERT(b)
    END Assert;
BEGIN
    str := "test22";
    Out.String("2.1: Length(str) ="); Out.Int(Length(str), 5); Out.Long(LEN(str), 5); Out.Ln;
    FillString(str);
    Out.String("2.2: Length(str) ="); Out.Int(Length(str), 5); Out.Long(LEN(str), 5); Out.Ln
END Run2;

PROCEDURE Run3();
VAR
    str : ARRAY 257 OF CHAR;
BEGIN
    str := "test333";
    Out.String("3.1: Length(str) ="); Out.Int(Length(str), 5); Out.Long(LEN(str), 5); Out.Ln;
    FillString(str);
    Out.String("3.2: Length(str) ="); Out.Int(Length(str), 5); Out.Long(LEN(str), 5); Out.Ln
END Run3;

BEGIN
    Run1();
    Run2();
    Run3()
END Issue37.