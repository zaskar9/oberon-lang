(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE Strings3;
IMPORT Out;

TYPE String = ARRAY 32 OF CHAR;
     StringList = ARRAY 10, 32 OF CHAR;

VAR list: StringList;

PROCEDURE Init(VAR list: ARRAY OF ARRAY OF CHAR);
VAR str: String;
BEGIN
    str := "Basel";
    list[0] := str;
    list[1] := "Zürich";
    list[2] := "Bern";
    list[3] := str;
    list[4] := "Basel";
    list[5] := "Lausanne";
    list[6] := "Zug";
    list[7] := "Lugano";
    list[8] := "Luzern";
    list[9] := "Chur";
END Init;

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

BEGIN
    Test1;
    Init(list);
    Print(list);
    Out.Ln
END Strings3.
(*
  CHECK: [Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda, Wakanda]
  CHECK: [Basel, Zürich, Bern, Basel, Basel, Lausanne, Zug, Lugano, Luzern, Chur]
*)