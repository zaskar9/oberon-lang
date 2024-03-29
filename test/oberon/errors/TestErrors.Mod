(* Test module containing syntax errors for the Oberon LLVM compiler. *)
MODULE 123;
IMPORT A, B := C, D E, F :=, G;

(* Length of the array to be sorted. *)
CONST Dim = 4 + 2 * -8;
      Float = 3.3.3;
      Huge = 10000000000000000000;
      Pi* = 3.14159268;


TYPE Point2D* = RECORD x, y: INTEGER END;
     Point3D* = RECORD x, y, y: INTEGER END;
     PointList = ARRAY 2.5 OF Point2D;
     ColorRGB* = RECORD r*, g*, b*: INTEGER END;
     Rect2D = RECORD color: ColorRGB; points*: PointList END;

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTEGER;
    i: INTEGER;
    point*: Point2D;
    bool: BOOLEAN;

PROCEDURE InitRect2D*(VAR rect: Rect2D; r, g, b: INTEGER; x, y: Point2D);
TYPE TRecord* = RECORD a, b*, c: INTEGER END;
BEGIN
    x.y := i;
    rect.color.r := r;
    rect.color.g := g;
    rect.color.b := TRUE;
    rect.points[].x := x.x;
    rect.points[0].y := x.y;
    rect.points[1].x := y.x;
    rect.points[1].y := x.y;
    rect.points[2].x := y.x;
    rect.points[2].y := y.y;
    rect.points[3].x := x.x;
    rect.points[3].y := y.y
END InitRect2D;

PROCEDURE CheckDimension(VAR dim: INTEGER) : BOOLEAN;
BEGIN
    IF dim < 1 THEN
        Dim := 1
    ELSIF dim > Dim THEN
        dim := Dim *;
        dim := Dim +
    ELSIF 0 # 1 THEN
        dim := Dim - 1;
    ELSE
        IF dim = dim THEN
            dim := -dim
        ELSIF dim # dim THEN
            dim := dim
        END
    END;
    RETURN dim
END CheckDimension;

PROCEDURE Test(VAR value: INTEGER);
CONST one* = 1;
VAR point: Point2D;
    i: INTEGER;
    foo: FooType;
    bar: foo;
    v1: "Hello World!";
    v2: ARRAY 1 > 2;
    v3: ARRAY one;
    v4: ARRAY;
BEGIN
    point.x := (1 + CheckDimension;
    point.y := -1;
    CheckDimension(1 + 2);
    CheckDimension(TRUE);
    WHILE 1 # 1 DO
        i := Dim;
        IF 1 > 2 THEN i := 1 ELSE i := 2 END
    END;
    RETURN point
END Test;

(* Initializes the array. *)
PROCEDURE Init();
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE i < Dim DO
        a[i] := Dim - i;
        i := i + 1
    END
END Init;

(* Initializes the array. *)
PROCEDURE Initialize(a b: INTEGER);
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO Dim BY 0 DO
        FOR j := Dim TO i BY -1 DO
            a[i] := j
        END
    END
END Initialise;

(* Swaps the two values passed as var-parameters. *)
PROCEDURE Swap(VAR a, b, : INTEGER);
VAR t*: INTEGER;
BEGIN
    IF a > b THEN END;
    t := a
    a := b; 
    b := t
END Swap;
    
(* Applies the bubble-sort algorithm to the array. *)
PROCEDURE BubbleSort;
VAR i, j: INTEGER;
BEGIN
    i := 0;
    WHILE i < Dim DO
        j := Dim - 1
        WHILE j > i DO
            j := Dim + 0 * 1
            j := Dim * 1
            j := Dim ! 1;
            IF a[j-1] > a[j] THEN Swap(a[j - 1], a[j]) END;
            j := j - 1
        END;
        i := i + 1
    END
END BubbleSort;

(* Applies the quick-sort algorithm to the array. *)
PROCEDURE QuickSort;

    PROCEDURE QSort*(l, r: INTEGER);
    VAR i, j x: INTEGER;
    BEGIN
        i := l;
        j := r;
        x := a[(r + l) DIV 2];
        WHILE i <= j DO
            WHILE a[i] < x DO i := i + 1 END;
            WHILE x < a[j] DO j := j - 1 END;
            IF i <= j THEN
                Swap(a[i], a[j])
                i := i + 1;
                j := j - 1
            END
        END;
        IF l < j THEN QSort(l, j) END;
        IF i < r THEN QSort(i, r) END
    END QSort;

BEGIN
    QSort(0, Dim - 1);
END QuickSort;

(* Main program. *)
BEGIN
    i := 1;
    bool := (i >= 0) OR (i < 100) & (point.x > 0);
    IF i != 2 THEN i := i + 1 END; (* interrupted by signal 11: SIGSEGV *)
    REPEAT
        Init();
        QuickSort();
        i := i + 1
    UNTIL i > Dim DIV 2;
    i = 1;
    Write("[");
    WHILE i <= Dim DO
        Write(a[i]);
        IF i + 1 < Dim THEN Write(", ") END;
        i := i + 1
    END;
    Write("]");
    WriteLn();
    RETURN i
END Tester.