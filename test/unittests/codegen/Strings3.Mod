(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Strings3;
IMPORT Out, SYSTEM;

CONST Len = 10;
      StrLen = 32;

TYPE String = ARRAY StrLen OF CHAR;
     StringList = ARRAY Len, StrLen OF CHAR;
     CharList = ARRAY Len * StrLen OF CHAR;

VAR list: StringList;

PROCEDURE Init(VAR list: ARRAY OF ARRAY OF CHAR);
VAR str: String;
BEGIN
    str := "Basel";
    list[0] := str;
    list[1] := "Zurich";
    list[2] := "Bern";
    list[3] := str;
    list[4] := "Basel";
    list[5] := "Lausanne";
    list[6] := "Zug";
    list[7] := "Lugano";
    list[8] := "Lucerne";
    list[9] := "Chur"
END Init;

PROCEDURE Swap(VAR x, y: String);
VAR t: String;
BEGIN
    t := x; x := y; y := t
END Swap;

PROCEDURE Print(list: ARRAY OF ARRAY OF CHAR);
VAR i: INTEGER;
BEGIN
    Out.Char("[");
    FOR i := 0 TO SHORT(LEN(list) - 1) DO
        Out.String(list[i]);
        IF i < LEN(list) - 1 THEN Out.String(", ") END
    END;
    Out.Char("]")
END Print;

PROCEDURE Sort(VAR list: ARRAY OF ARRAY OF CHAR);
VAR i, n, len: INTEGER;
BEGIN
    len := SHORT(LEN(list));
    REPEAT
        n := 0;
        FOR i := 1 TO len - 1 DO
            IF list[i - 1] > list[i] THEN
                Swap(list[i - 1], list[i]);
                n := i
            END
        END;
        len := n
    UNTIL n <= 1
END Sort;

PROCEDURE Test1;
VAR list: StringList;
    str: String;
    i: INTEGER;
BEGIN
    str := "Wakanda";
    FOR i := 0 TO SHORT(LEN(list) - 1) DO
        list[i] := str
    END;
    Print(list); Out.Ln
END Test1;

PROCEDURE Dump(list: StringList);
VAR chars: CharList;
    ch: CHAR;
    i: INTEGER;
BEGIN
    chars := SYSTEM.VAL(CharList, list);
    FOR i := 0 TO SHORT(LEN(chars) - 1) DO
        IF i MOD StrLen = 0 THEN Out.Char("[") END;
        ch := chars[i];
        IF (ch >= "A") & (ch <= "Z") OR (ch >= "a") & (ch <= "z") THEN
            Out.Char(ch)
        ELSE
            Out.Char(".")
        END;
        IF (i + 1) MOD StrLen = 0 THEN
            Out.Char("]"); Out.Ln
        END
    END
END Dump;

BEGIN
    Test1;
    Init(list);
    Print(list); Out.Ln;
    Sort(list);
    Print(list); Out.Ln;
    ASSERT("Z" > list[0]);
    ASSERT("Zurich" >= list[9]);
    ASSERT("A" < "Z");
    ASSERT("A" <= "Aa")
END Strings3.
(*
  CHECK: [Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda]
  CHECK: [Basel, Zurich, Bern, Basel, Basel, Lausanne, Zug, Lugano, Lucerne, Chur]
  CHECK: [Basel, Basel, Basel, Bern, Chur, Lausanne, Lucerne, Lugano, Zug, Zurich]
*)