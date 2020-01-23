MODULE CodeGen;

CONST Pi = 3;

VAR x, y, z: INTEGER;

BEGIN
    x := 10;
    y := 3;
    z := x DIV (y + 1);
    x := (Pi * z - y) * (Pi * z - y);
    y := x * (1 DIV z)
    (* Write(x); *)
    (* Write(y); *)
    (* Write(z); *)
END CodeGen.