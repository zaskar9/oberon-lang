MODULE CodeGen;

CONST Dim = 20;

TYPE Point2D = RECORD x, y: INTEGER END;
     Point3D = RECORD
                  p2d: Point2D;
                  z: INTEGER
               END;
     PointList = ARRAY Dim OF Point3D;
     ColorRGB = RECORD
                   r, g, b: INTEGER
                END;
     Polygon3D = RECORD
                    points: PointList;
                    color: ColorRGB
                 END;

VAR x, y, z, i: INTEGER;
    a: ARRAY Dim OF INTEGER;
    p2d: Point2D;
    p3d: Point3D;
    points: PointList;
    poly3d: Polygon3D;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

(* Import `rand` function from C <stdlib.h> library. *)
PROCEDURE rand(): INTEGER; EXTERN;

(* Writes a string value to standard out. *)
PROCEDURE Write(val: STRING);
BEGIN
    printf(val)
END Write;

(* Writes a newline to standard out. *)
PROCEDURE WriteLn;
BEGIN
    printf("\n")
END WriteLn;

(* Writes an integer value to standard out. *)
PROCEDURE WriteInt(val: INTEGER);
BEGIN
    printf("%d", val)
END WriteInt;

(* Writes an integer tuple to standard out. *)
PROCEDURE WriteTuple(x, y: INTEGER);
BEGIN
    printf("(%d, %d)", x, y)
END WriteTuple;

(* Writes an integer triple to standard out. *)
PROCEDURE WriteTriple(x, y, z: INTEGER);
BEGIN
    printf("(%d, %d, %d)", x, y, z)
END WriteTriple;

(* Computes the maximum of two numbers. *)
PROCEDURE max(x, y: INTEGER): INTEGER;
VAR res: INTEGER;
BEGIN
    IF x > y THEN res := x ELSE res := y END;
    RETURN res
END max;

(* Compares two numbers. *)
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

(* Recursive computation of greatest common denominator. *)
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

(* Recursive computation of n-th Fibonacci number. *)
PROCEDURE rFib(n: INTEGER): INTEGER;
BEGIN
    IF n > 1 THEN
        RETURN rFib(n - 2) + rFib(n - 1)
    ELSE
        RETURN n
    END
END rFib;

(* Recursive computation of factorial. *)
PROCEDURE rFac(n: INTEGER): INTEGER;
BEGIN
    IF n > 1 THEN
        RETURN n * rFac(n - 1)
    ELSE
        RETURN n
    END
END rFac;

(* Iterative computation of factorial. *)
PROCEDURE iFac(n: INTEGER): INTEGER;
VAR i, res: INTEGER;
BEGIN
    res := n;
    FOR i := n - 1 TO 1 BY -1 DO
        res := res * i
    END;
    RETURN res
END iFac;

(* Swaps the two values passed as var-parameters. *)
PROCEDURE Swap(VAR x, y: INTEGER);
VAR t: INTEGER;
BEGIN
    t := x;
    x := y;
    y := t
END Swap;

(* Initializes the array. *)
PROCEDURE Init();
VAR i: INTEGER;
BEGIN
    i := 0;
    REPEAT
        a[i] := rand() MOD 100;
        i := i + 1
    UNTIL i = Dim
END Init;

(* Prints the array. *)
PROCEDURE Print();
VAR i: INTEGER;
BEGIN
    Write("[");
    i := 0;
    WHILE i < Dim DO
        WriteInt(a[i]);
        IF i + 1 < Dim THEN Write(", ") END;
        i := i + 1
    END;
    Write("]");
    WriteLn()
END Print;

(* Applies the bubble-sort algorithm to the array. *)
PROCEDURE BubbleSort;
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO Dim - 1 DO
        FOR j := Dim - 1 TO i + 1 BY -1 DO
            IF a[j - 1] > a[j] THEN
                Swap(a[j - 1], a[j])
            END
        END
    END
END BubbleSort;

PROCEDURE CreatePoint2D(x, y: INTEGER): Point2D;
VAR p: Point2D;
BEGIN
    p.x := x;
    p.y := y;
    RETURN p
END CreatePoint2D;

PROCEDURE CreatePoint3D(x, y, z: INTEGER): Point3D;
VAR p: Point3D;
BEGIN
    p.p2d.x := x;
    p.p2d.y := y;
    p.z := z;
    RETURN p
END CreatePoint3D;

PROCEDURE CreatePoints(VAR points: PointList);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO Dim - 1 DO
        points[i] := CreatePoint3D(rand() MOD 1000, rand() MOD 1000, rand() MOD 1000)
    END
END CreatePoints;

PROCEDURE WritePoint3D(p3d: Point3D);
VAR i: INTEGER;
BEGIN
    WriteTriple(p3d.p2d.x, p3d.p2d.y, p3d.z)
END WritePoint3D;

PROCEDURE WriteColor(col: ColorRGB);
BEGIN
    WriteTriple(col.r, col.g, col.b)
END WriteColor;

PROCEDURE WritePoints(VAR points: PointList);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO Dim - 1 DO
        WritePoint3D(points[i]); WriteLn
    END
END WritePoints;

PROCEDURE CreatePoly3D(r, g, b: INTEGER): Polygon3D;
VAR poly3d: Polygon3D;
BEGIN
    CreatePoints(poly3d.points);
    poly3d.color.r := r;
    poly3d.color.g := g;
    poly3d.color.b := b;
    RETURN poly3d
END CreatePoly3D;

PROCEDURE parent(x, y: INTEGER): INTEGER;
VAR parent_i: INTEGER;

    PROCEDURE _child(z: INTEGER): INTEGER;
    VAR j: INTEGER;

        PROCEDURE _grand_child(u, v: INTEGER): INTEGER;
        VAR w: INTEGER;

        BEGIN
            w := parent_i;
            RETURN u + v + w + j
        END _grand_child;

    BEGIN
        j := _grand_child(parent_i, z);
        RETURN j
    END _child;

BEGIN
    parent_i := x;
    RETURN _child(y)
END parent;


BEGIN
    x := 27;
    y := 9;
    z := 3;
    WriteTuple(x, y); WriteLn;
    Swap(x, y);
    WriteTuple(x, y); WriteLn;
    WriteInt(gcd(x, y)); WriteLn;
    WriteInt(max(x, y)); WriteLn;
    WriteInt(compare(x, y)); WriteLn;
    WriteInt(compare(y, x)); WriteLn;
    WriteInt(rFib(x)); WriteLn;
    WriteInt(rFac(z)); WriteLn;
    WriteInt(iFac(z)); WriteLn;
    poly3d := CreatePoly3D(x, y, z);
    WritePoints(poly3d.points);
    Init;
    Print;
    BubbleSort;
    Print;
    WriteInt(parent(x, y))
END CodeGen.

(* ./oberon0c CodeGen.Mod && opt -S -O3 CodeGen.ll | llc -O3 -o CodeGen.s && g++ CodeGen.s -o CodeGen && ./CodeGen *)