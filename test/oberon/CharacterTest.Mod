MODULE CharacterTest;
IMPORT Out;

VAR string: ARRAY 256 OF CHAR;
    i: INTEGER;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

BEGIN
    string[0] := "O";
    string[1] := "b";
    string[2] := "e";
    string[3] := "r";
    string[4] := "o";
    string[5] := "n";
    string[6] := "\0";

    (* Out.String(string); Out.Ln; *)
    FOR i := 0 TO 6 DO
        Out.Char(string[i])
    END;
    Out.Ln

END CharacterTest.