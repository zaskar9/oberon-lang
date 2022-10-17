MODULE Floats;
IMPORT Out;

VAR float: REAL;
    double: LONGREAL;
    int: INTEGER;

PROCEDURE dummy1(p1: INTEGER): INTEGER; EXTERN;

PROCEDURE dummy2(p1: INTEGER; p2: INTEGER): INTEGER; EXTERN;

BEGIN
    int := dummy2(1, 2);
    int := dummy1(int);
    float := 3.141592654;
    double := float;
    int := float (INTEGER);
    Out.Real(float); Out.Ln;
    Out.Int(int); Out.Ln
END Floats.