MODULE CharacterTest;
IMPORT Out;

TYPE TString = ARRAY 256 OF CHAR;

VAR s1: TString;
    s2: STRING;
    i: INTEGER;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE OutString(s: TString);
BEGIN
    Out.String(s)
END OutString;

BEGIN
    s1[0] := "O";
    s1[1] := "b";
    s1[2] := "e";
    s1[3] := "r";
    s1[4] := "o";
    s1[5] := "n";
    s1[6] := "!";
    s1[7] := "\0";

    s2 := s1;

    (* OutString(s1); Out.Ln; *)
    Out.String(s2); Out.Ln;


    (* FOR i := 0 TO 6 DO
        Out.Char(s1[i])
    END; *)
    Out.Ln

END CharacterTest.