MODULE Springer;

IMPORT Out;

CONST n = 3;

TYPE Feld = ARRAY n, n OF BOOLEAN;
        Platz = RECORD
                    X : INTEGER;
                    Y : INTEGER;
                  END;
        Loesung= ARRAY n*n OF Platz;
        
VAR Spielbrett : Feld;

PROCEDURE ErlaubterZug(Zug : Platz) : BOOLEAN;
BEGIN
    IF ((Zug.X>=0) & (Zug.X<n)) & ((Zug.Y>=0) & (Zug.Y<n)) & ( ~Spielbrett[Zug.X, Zug.Y] )
        THEN RETURN TRUE;
        ELSE RETURN FALSE;
    END;
END ErlaubterZug;

PROCEDURE Ziehen(i : INTEGER; Zug : Platz; VAR NZug : Platz);
BEGIN
    CASE i OF    
        1 : NZug.X:=Zug.X+2; NZug.Y:=Zug.Y+1;
        | 2 : NZug.X:=Zug.X+1; NZug.Y:=Zug.Y+2;
        | 3 : NZug.X:=Zug.X-1; NZug.Y:=Zug.Y+2;
        | 4 : NZug.X:=Zug.X-2; NZug.Y:=Zug.Y+1;
        | 5 : NZug.X:=Zug.X-2; NZug.Y:=Zug.Y-1;
        | 6 : NZug.X:=Zug.X-1; NZug.Y:=Zug.Y-2;
        | 7 : NZug.X:=Zug.X+1; NZug.Y:=Zug.Y-2;
        | 8 : NZug.X:=Zug.X+2; NZug.Y:=Zug.Y-1;
    ELSE NZug:=Zug;
    END;
END Ziehen;

PROCEDURE KleinsteAlternative(Zuvor : INTEGER; Position : Platz) : INTEGER;
VAR NewPos : Platz;
BEGIN
    REPEAT
        INC(Zuvor);
        Ziehen(Zuvor, Position, NewPos);
    UNTIL (ErlaubterZug(NewPos)) OR (Zuvor > 8);
    RETURN Zuvor;
END KleinsteAlternative;

PROCEDURE FindeLoesung(i : INTEGER; VAR Gefunden : BOOLEAN; VAR L : Loesung);
VAR Alternative : INTEGER;
BEGIN
    Gefunden:=FALSE;
    Alternative:=KleinsteAlternative(0, L[i]);
    Out.Int(Alternative, 1); Out.Int(i, 3);
    LOOP IF Alternative <= 8 THEN
                    Ziehen(Alternative, L[i], L[i+1]);
                    Out.Int(L[i+1].X,5); Out.Int(L[i+1].Y,3); Out.Ln;
                    Spielbrett[L[i+1].X,L[i+1].Y]:=TRUE;
                    IF i+1>=(n*n)-1                                                (* letztes Element im Array *) 
                        THEN Gefunden:=TRUE; EXIT;
                        ELSE FindeLoesung(i+1, Gefunden, L);
                    END;
                    IF Gefunden 
                        THEN EXIT;
                        ELSE Spielbrett[L[i+1].X, L[i+1].Y]:=FALSE;
                    END;
            END;
            IF Alternative < 8 
                THEN Alternative:=KleinsteAlternative(Alternative, L[i]);
                ELSE EXIT;
            END;
    END;
END FindeLoesung;

PROCEDURE SpringerWeg*();
VAR Solution : Loesung;
        Found : BOOLEAN;
        i, j : INTEGER;
BEGIN
    FOR i:=0 TO n-1 DO
        FOR j:=0 TO n-1 DO
            Spielbrett[i, j]:=FALSE;
        END;
    END;
    Solution[0].X:=0;
    Solution[0].Y:=0;
    Spielbrett[0,0]:=TRUE;
    FindeLoesung(0, Found, Solution);
    IF Found THEN Out.String("Weg gefunden");
                ELSE Out.String("Kein Weg gefunden");
    END;
END SpringerWeg;

END Springer.