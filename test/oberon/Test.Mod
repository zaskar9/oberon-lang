MODULE Test;
IMPORT Out;

TYPE String = ARRAY 255 OF CHAR;
     Shape = POINTER TO ShapeDesc;
     ShapeDesc = RECORD area: REAL END;
     Circle = POINTER TO CircleDesc;
     CircleDesc = RECORD (ShapeDesc) x, y, r: REAL END;
     Person = POINTER TO PersonDesc;
     PersonDesc = RECORD first, last: String END;

VAR s: Shape;
    sd: ShapeDesc;
    c: Circle;
    cd: CircleDesc;
    (* p: Person; *)
    pd: PersonDesc;

PROCEDURE Dereference(VAR sd: ShapeDesc; s: Shape);
BEGIN
    sd := s^
END Dereference;

(* ---
PROCEDURE Test;
VAR s: Shape;
    sd: ShapeDesc;
BEGIN
    NEW(s);
    s.area := 0.0;
    Dereference(sd, s);
    DISPOSE(s)
END Test;
--- *)

BEGIN
    Out.String("--- Shape"); Out.Ln;
    NEW(s);
    s.area := 1.0;
    Out.Real(s.area, 0); Out.Ln;
    Dereference(sd, s);
    DISPOSE(s);
    Out.Real(sd.area, 0); Out.Ln;
    Out.String("--- Circle"); Out.Ln;
    NEW(c);
    c.area := 1.0;
    c.x := 2.0;
    c.y := 3.0;
    c.r := 4.0;
    Out.Real(c.area, 0); Out.Ln;
    Out.Real(c.x, 0); Out.Ln;
    Out.Real(c.y, 0); Out.Ln;
    Out.Real(c.r, 0); Out.Ln;
    (* Dereference(cd, c); *)
    cd := c^;
    DISPOSE(c);
    Out.Real(cd.area, 0); Out.Ln;
    Out.Real(cd.x, 0); Out.Ln;
    Out.Real(cd.y, 0); Out.Ln;
    Out.Real(cd.r, 0); Out.Ln
END Test.