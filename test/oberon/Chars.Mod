MODULE Chars;
IMPORT Out;

TYPE String = ARRAY 256 OF CHAR;
     ShortStr = ARRAY 100 OF CHAR;
     PersonDesc = RECORD
        first: STRING;
        last: String
     END;
     Person = POINTER TO PersonDesc;

VAR s1, s2: String;
    s3: ShortStr;
    s4, s5: STRING;
    i: INTEGER;
    ch: CHAR;
    p1, p2: Person;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE OutString(s: ARRAY OF CHAR);
BEGIN
    printf("%s", s)
END OutString;

PROCEDURE OutString0(s: ARRAY OF CHAR);
VAR i: LONGINT;
BEGIN
    i := 0;
    WHILE (i < LEN(s)) & (s[i] # "\0") DO
        Out.Char(s[i]);
        INC(i)
    END
END OutString0;

BEGIN
    s1[0] := 4FX;
    s1[1] := 62X;
    s1[2] := "e";
    s1[3] := "r";
    s1[4] := "o";
    s1[5] := "n";
    s1[6] := "!";
    s1[7] := "\0";

    s2 := s1;
    s2[6] := "?";

    s3 := "Hello World!";

    s4 := "Hoi Niklaus!";
    s5 := s4;

    printf("s1: %s\n", s1);
    printf("s2: %s\n", s2);
    printf("s3: %s\n", s3);
    printf("s4: %s\n", s4);
    printf("s5: %s\n\n", s5);

    OutString(s1); Out.Ln;
    OutString0(s1); Out.Ln; Out.Ln;

    OutString("This is a test."); Out.Ln;
    OutString0("This is a test."); Out.Ln; Out.Ln;

    Out.Long(LEN("This is a test."), 0); Out.Ln; Out.Ln;

    Out.String("["); Out.Char("]"); Out.Ln;

    NEW(p1);
    p1.first := "Harry";
    p1.last := "Potter";
    Out.String(p1.first); Out.String(" "); OutString(p1.last); Out.Char("\n");
    FREE(p1)


END Chars.