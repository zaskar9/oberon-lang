MODULE CodeGen;

VAR x, y: INTEGER;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;
PROCEDURE rand(): INTEGER; EXTERN;

PROCEDURE WriteInt(val: INTEGER);
BEGIN
    printf("%d\n", val)
END WriteInt;

PROCEDURE WriteTuple(x, y: INTEGER);
BEGIN
    printf("(%d, %d)\n", x, y)
END WriteTuple;

PROCEDURE Swap(VAR x, y: INTEGER);
VAR t: INTEGER;
BEGIN
    t := x;
    x := y;
    y := t
END Swap;

PROCEDURE max(x, y: INTEGER): INTEGER;
VAR res: INTEGER;
BEGIN
    IF x > y THEN res := x ELSE res := y END;
    RETURN res
END max;

PROCEDURE compare(x, y: INTEGER): INTEGER;
VAR res: INTEGER;
BEGIN
    IF x > y THEN
        res := 1
    ELSIF x < y THEN
        res := -1
    ELSE
        res := 0
    END;
    RETURN res
END compare;

PROCEDURE gcd(x, y: INTEGER): INTEGER;
BEGIN
    IF x = y THEN
        RETURN x
    ELSIF x < y THEN
        RETURN gcd(x , y - x)
    ELSE
        RETURN gcd(x - y, y)
    END
END gcd;

PROCEDURE rFib(n: INTEGER): INTEGER;
BEGIN
    IF n > 1 THEN
        RETURN rFib(n - 2) + rFib(n - 1)
    ELSE
        RETURN n
    END
END rFib;

PROCEDURE rFac(n: INTEGER): INTEGER;
BEGIN
    IF n > 1 THEN
        RETURN n * rFac(n - 1)
    ELSE
        RETURN n
    END
END rFac;

PROCEDURE test: INTEGER;
BEGIN
    WriteInt(5)
END test;

PROCEDURE iFac(n: INTEGER): INTEGER;
VAR i, res: INTEGER;
BEGIN
    res := n;
    FOR i := n - 1 TO 1 BY -1 DO
        res := res * i
    END;
    RETURN res
END iFac;

BEGIN
    (* x := 12;
    y := 9;
    WriteTuple(x, y);
    Swap(x, y);
    WriteTuple(x, y);
    WriteInt(gcd(x, y));
    WriteInt(max(x, y));
    WriteInt(compare(x, y));
    WriteInt(compare(y, x));
    WriteInt(rFib(10));
    WriteInt(rFac(5));
    WriteInt(iFac(5)) *)
    test()
END CodeGen.

(* llc -O0 CodeGen.ll && g++ CodeGen.s -o CodeGen && ./CodeGen *)
(* opt -S -O3 CodeGen.ll -o CodeGen-opt.ll *)