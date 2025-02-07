MODULE TrapCopyOverflow;
IMPORT Out;

TYPE Vector = ARRAY 10 OF INTEGER;
     List = ARRAY 20 OF INTEGER;

VAR v: Vector;
    l: List;
    i: INTEGER;
    str: ARRAY 10 OF CHAR;

PROCEDURE assign(VAR v: Vector; a: ARRAY OF INTEGER);
BEGIN
    v := a
END assign;

PROCEDURE assignStr(VAR dst: ARRAY OF CHAR; src: ARRAY OF CHAR);
BEGIN
    dst := src
END assignStr;

BEGIN
    assignStr(str, "Hello World!");
    Out.String(str);
    (* FOR i := 0 TO 19 DO
        l[i] := i
    END;
    assign(v, l);
    FOR i := 0 TO 9 DO
        Out.Int(v[i], 0); Out.Char(" ")
    END; *)
    Out.Ln
END TrapCopyOverflow.