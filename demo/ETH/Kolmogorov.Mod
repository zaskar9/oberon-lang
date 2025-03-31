(* Informatikübbungen: Serie 4, 28.11.96, Michael Grossniklaus *)

MODULE Kolmogorov;

IMPORT In, Out;

TYPE Reihe = ARRAY 1000 OF BOOLEAN;

PROCEDURE CalculateSeries(Index, n : INTEGER; VAR A : Reihe);
VAR i, j, k, h, Counter : INTEGER;
BEGIN
    i:=1; k:=1; h:=1; Counter:=0;
    IF Index=1 
        THEN
        (* Ich glaube nicht, dass es grossen Sinn macht, bei solchen Zählerschleifen Invarinaten zu suchen. *)
            WHILE (i <= n) DO
                j:=1;
                WHILE (j <= i) & (Counter < n) DO
                    IF ODD(i) THEN A[Counter+1]:=TRUE;
                                     ELSE A[Counter+1]:=FALSE,
                    END;
                    INC(j);
                    INC(Counter)
                END;
                INC(i);
            END;
        ELSIF Index=2
            THEN
                WHILE (i <= n) DO
                    j:=1;
                    WHILE (j <= k) & (Counter < n) DO
                        A[Counter+1]:=TRUE;
                        INC(j);
                        INC(Counter);
                    END;
                    k:=k+h;
                    h:=j-1;
                    j:=1;
                    WHILE (j <= i) & (Counter < n) DO
                        A[Counter+1]:=FALSE;
                        INC(j);
                        INC(Counter);
                    END;
                    INC(i);
                END;
            END; (* of IF...THEN...ELSE *)    
END CalculateSeries;

PROCEDURE Ausgabe(Index, n : INTEGER);
VAR i : INTEGER;
       ReihenArray : Reihe;
BEGIN
    CalculateSeries(Index, n, ReihenArray);
    i:=1;
    WHILE i <= n DO
        IF ReihenArray[i] THEN Out.Int(1,0);
                                ELSE Out.Int(0,0);
        END;
        INC(i);
    END;
END Ausgabe;


PROCEDURE Reihe1*();
VAR n : INTEGER;
BEGIN 
    In.Open;
    In.Int(n);
    Ausgabe(1,n);
END Reihe1;            

PROCEDURE Reihe2*();
VAR n : INTEGER;
BEGIN 
    In.Open;
    In.Int(n);
    Ausgabe(2,n);
END Reihe2;

PROCEDURE Reihe3*();
VAR RArray1, RArray2 : Reihe;
       n, i : INTEGER;            
BEGIN 
    In.Open;
    In.Int(n);
    CalculateSeries(1, n, RArray1);
    CalculateSeries(2, n, RArray2);
    i:=1;
    WHILE i <= n DO
        IF (RArray1[i] & RArray2[i]) OR (~ RArray1[i] & ~ RArray2[i]) THEN Out.Int(1,0);
                                                                                             ELSE Out.Int(0,0);
        END;
        INC(i);
    END;
END Reihe3;

END Kolmogorov.