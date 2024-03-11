MODULE Vigenere;
IMPORT Out;

CONST Len = 256;

TYPE
    TString = RECORD
        Text: ARRAY Len OF CHAR;
        Length: INTEGER
    END;

(* Liest einen String ein und hält seine Länge fest; Leerschläge am Anfang werden ignoriert *)
(*PROCEDURE ReadString(VAR StrOut: TString);
VAR ch: CHAR;
    i, j: INTEGER;
BEGIN
    In.Open;
    i := 0; j := 0;
    REPEAT
        In.Char(ch)
    UNTIL ch # " ";
    WHILE (In.Done) & (ch # "~") & (i < 255) DO
        StrOut.Text[i] := ch;
        In.Char(ch);
        INC(i)
    END;
    StrOut.Length := i
END ReadString;*)

(* Schreibt einen String des Typs TString *)
PROCEDURE WriteString(StrIn: TString);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO StrIn.Length - 1 DO
        Out.Char(StrIn.Text[i])
    END
END WriteString;

PROCEDURE Cut(VAR StrIn, StrOut: TString; SCh: CHAR);
(* Schneidet ein Segment bis zu einem gewissen Zeichen aus einem String heraus *)
VAR StrNew: TString;
    i, j: INTEGER;
    ch: CHAR;
BEGIN
    i := 0;
    WHILE StrIn.Text[i] # SCh DO
        StrOut.Text[i] := StrIn.Text[i];
        INC(i)
    END;
    StrOut.Length := i;
    j := 0;
    FOR i := StrOut.Length + 1 TO StrIn.Length DO
        INC(j);
        StrNew.Text[j - 1] := StrIn.Text[i]
    END;
    StrNew.Length := j - 1;
    StrIn := StrNew
END Cut;

(* Liest den ganzen Eingabestring und zerteilt ihn in Schlüsselwort und Text *)
(*PROCEDURE Einlesen(VAR Txt, Key : TString);
BEGIN
    ReadString(Txt);
    Cut(Txt, Key, " ")
END Einlesen;*)

PROCEDURE NormORD(ch : CHAR): INTEGER;
BEGIN
    RETURN ORD(ch)-ORD("A")
END NormORD;

PROCEDURE NormCHR(i : INTEGER): CHAR;
BEGIN
    RETURN CHR(i + ORD("A"))
END NormCHR;

PROCEDURE Transponieren(VAR InString: TString);
(* Tansponiert die Matrix in der der String enthalten ist *)

VAR i, j, k, n: INTEGER;
    OutString: TString;
BEGIN
    k := 1;
    WHILE k * k < InString.Length DO
        INC(k)
    END;
    FOR i := InString.Length TO (k * k) - 1 DO
        IF i < Len - 1 THEN InString.Text[i] := " " END
    END;
    InString.Length := k * k;
    i:= 0; j:= 0; n:=0;
    WHILE j < InString.Length DO
        OutString.Text[j]:=InString.Text[i];
        INC(j); i:=i+k;
        IF i  > InString.Length-1 THEN
            INC(n);
            i:=n
        END
    END;
    OutString.Length := InString.Length;
    InString := OutString
END Transponieren;

PROCEDURE Codieren(Code : BOOLEAN; VAR KlTxt, Schlu, Chffr : TString);
(* Codiert bzw. Decodiert den übergebenen Text *)

VAR i, j : INTEGER;
BEGIN
    IF Code THEN
        j:= 0;
        FOR i:= 0 TO KlTxt.Length-1 DO
            IF (ORD(KlTxt.Text[i]) >= ORD("A")) & (ORD(KlTxt.Text[i]) <= ORD("Z")) THEN
                Chffr.Text[i]:=NormCHR((NormORD(KlTxt.Text[i]) + NormORD(Schlu.Text[j MOD Schlu.Length]))  MOD 26);
                INC(j)
            ELSE Chffr.Text[i]:=KlTxt.Text[i]
            END
        END;
        Chffr.Length:=KlTxt.Length;
        Transponieren(Chffr)
    ELSE
        Transponieren(Chffr);
        j:= 0;
        FOR i:= 0 TO Chffr.Length - 1 DO
            IF (ORD(Chffr.Text[i]) >= ORD("A")) & (ORD(Chffr.Text[i]) <= ORD("Z")) THEN
                KlTxt.Text[i] := NormCHR((26 + NormORD(Chffr.Text[i]) - NormORD(Schlu.Text[j MOD Schlu.Length])) MOD 26);
                INC(j)
            ELSE KlTxt.Text[i] := Chffr.Text[i]
            END
        END;
        KlTxt.Length := Chffr.Length
    END
END Codieren;

PROCEDURE StrLen(s: ARRAY OF CHAR): INTEGER;
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE (i < Len - 1) & (s[i] # "\0") DO
        INC(i)
    END;
    RETURN i
END StrLen;

(* Chiffriert den Text *)
PROCEDURE Chiffrieren*(key, text: ARRAY OF CHAR);
VAR Klartxt, Schl, Chiff: TString;
BEGIN
    (*Einlesen(Klartxt, Schl);*)
    Klartxt.Text := text; Klartxt.Length := StrLen(text);
    Schl.Text := key; Schl.Length := StrLen(key);
    Codieren(TRUE, Klartxt, Schl, Chiff);
    WriteString(Chiff); Out.Char("~"); Out.Ln
END Chiffrieren;

(* Dechiffriert den Text *)
PROCEDURE Dechiffrieren*(key, text: ARRAY OF CHAR);
VAR Klartxt, Schl, Chiff : TString;
BEGIN
    (*Einlesen(Chiff, Schl);*)
    Chiff.Text := text; Chiff.Length := StrLen(text);
    Schl.Text := key; Schl.Length := StrLen(key);
    Codieren(FALSE, Klartxt, Schl, Chiff);
    WriteString(Klartxt); Out.Char("~"); Out.Ln
END Dechiffrieren;

BEGIN
    Chiffrieren("LEMON", "ATTACKATDAWN");
    Dechiffrieren("LEMON", "acd tka taw atn ")
END Vigenere.