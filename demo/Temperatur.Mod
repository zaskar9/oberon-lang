(* Informatikübungen: Serie 3, Michael Grossniklaus, 22.11.1996 *)

MODULE Temperatur;

IMPORT In, Out;

(* Der Einfachheit halber habe ich die Prozedur RealFix des Moduls Ausgabe in mein Modul Temperatur kopiert *)

PROCEDURE RealFix(x: REAL; n, m: INTEGER);
        VAR mul: LONGINT;
    BEGIN
        IF x < 0 THEN Out.Char("-"); x := -x END;
        IF m > 0 THEN
            mul := 1;
            REPEAT mul := mul*10; DEC(m) UNTIL m = 0;
            Out.Int(ENTIER(x+(0.5/mul)), n);
            Out.Char(".");
            IF ENTIER(mul*x+0.5) MOD mul = 0 THEN
                REPEAT mul := mul DIV 10; Out.Char("0") UNTIL mul = 1
            ELSE Out.Int(ENTIER(mul*x+0.5) MOD mul, 0)
            END
        ELSE Out.Int(ENTIER(x+0.5), n)
        END
    END RealFix;

(* Hier beginnen die Prozeduren und Funktionen, die ich geschriben habe *)

PROCEDURE FC(f: REAL): REAL;
BEGIN
    RETURN (f-32)*(5/9)
END FC;

PROCEDURE CF(c: REAL):REAL;
BEGIN
    RETURN (9/5)*c+32;
END CF;

PROCEDURE Tabelle*();
VAR v, b, s : INTEGER;
BEGIN 
    In.Open;
    In.Int(v); In.Int(b); In.Int(s);
    
    (* Vorbedingung: s > 0 *)
    
    WHILE v<=b DO
        RealFix(v,3,2);
        RealFix(FC(v),8,2);        (* Invariante v <= v+s *)
        Out.Ln;
        v:=v+s;
    END
    
    (* Nachbedingung: v > b *)
    
END Tabelle;

PROCEDURE BessereTabelle*();
VAR v, b, s : INTEGER;
        i : REAL;
BEGIN 
    In.Open;
    In.Int(v); In.Int(b); In.Int(s);
    
    (* Vorbedingung: s > 0 *)
    
    WHILE v<=b DO    
        RealFix(v,3,2); RealFix(FC(v),8,2);
        Out.Ln;
        IF v+s<=b
            THEN
                i:=ENTIER((FC(v+s)+FC(v))/2+0.5);        (* Invariante: v <= v+s *)
                RealFix(CF(i),3,2); RealFix(i,8,2);
                Out.Ln;
            END;
        v:=v+s;
    END
    
    (* Nachbedingung: v > b *)
        
END BessereTabelle;

END Temperatur.