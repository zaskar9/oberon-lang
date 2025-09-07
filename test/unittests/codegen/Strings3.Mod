MODULE Strings3;
IMPORT Out;

TYPE Matrix = ARRAY 10, 10 OF INTEGER;

VAR list: ARRAY 10, 32 OF CHAR;
    arr1: ARRAY 10 OF INTEGER;
    arr2: ARRAY 10, 10 OF INTEGER;
    arr3: Matrix;
(*
PROCEDURE Init(VAR list: ARRAY OF ARRAY OF CHAR);
VAR str: ARRAY 32 OF CHAR;
BEGIN
    str := "Basel";
    list[0] := str;
    list[1] := "Zürich";
    list[2] := "Bern";
    list[4] := "Basel";
    list[5] := "Lausanne";
    list[6] := "Zug";
    list[7] := "Lugano";
    list[8] := "Luzern";
    list[9] := "Chur";
    Out.String(list[6]); Out.Ln;
    Out.String(str); Out.Ln
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

PROCEDURE Test1(VAR arr2: Matrix);
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO 9 DO
        FOR j := 0 TO 9 DO arr2[i, j] := i + j END;
    END;
    FOR i := 0 TO 9 DO
        FOR j := 0 TO 9 DO
            Out.Int(arr2[i, j], 3)
        END;
        Out.Ln
    END
END Test1;
*)
PROCEDURE Test2(VAR list: ARRAY OF ARRAY OF INTEGER; str: ARRAY OF INTEGER);
BEGIN
    list[0] := str
END Test2;

(* PROCEDURE Test3(VAR dst: ARRAY OF INTEGER; src: ARRAY OF INTEGER);
BEGIN
    dst := src
END Test3; *)

(* PROCEDURE Test;
VAR list: ARRAY 10, 10 OF CHAR;
    str: ARRAY 10 OF CHAR;
BEGIN
    str := "Wakanda";
    list[0] := str;
    Out.String(list[0]); Out.Ln
END Test; *)

BEGIN
    (* Out.String("--0--"); Out.Ln;
    Test;
    Out.String("--1--"); Out.Ln;
    Test1(arr3); *)
    Out.String("--2--"); Out.Ln;
    Test2(arr2, arr1);
    (* Out.String("--3--"); Out.Ln;
    Init(list);
    Out.String("--4--"); Out.Ln;
    Print(list); Out.Ln *)
END Strings3.