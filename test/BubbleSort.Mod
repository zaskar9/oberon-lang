(* Implementation of the Bubble Sort algorithm. *)
MODULE Sort;

(* Length of the array to be sorted. *)
CONST Dim = 4 + 2 * 8;

TYPE Point2D = RECORD x, y: INTEGER END;
     (* Point3D = RECORD x, y, y: INTEGER END; *)

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTEGER;
    i: INTEGER;
    point: Point2D;
(*
PROCEDURE CheckDimension(VAR dim: INTEGER);
BEGIN
    IF dim < 1 THEN
        Dim := 1
    ELSIF dim > Dim THEN
        dim := Dim +
    ELSIF 0 # 1 THEN
        dim := Dim - 1;
    ELSE
        IF dim = dim THEN
            dim := -dim
        ELSIF dim # dim THEN
            dim := dim
        END
    END
END CheckDimension;

PROCEDURE Test(VAR value: INTEGER);
VAR point: Point2D;
    i: INTEGER;
    foo: FooType;
    bar: foo;
    v1: ++;
    v2: ARRAY 1 > 2;
    v3: ARRAY 1;
    v4: ARRAY;
BEGIN
    point.x := (1 + CheckDimension;
    point.y := -1;
    WHILE 1 # 1 DO
        i := Dim;
        IF 1 > 2 THEN i := 1 ELSE i := 2 END
    END
END Test;
*)

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

PROCEDURE Initialize();
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO Dim DO
        FOR j := Dim TO i BY -1 DO
            a[i] := j
        END
    END
END Initialize;

(* Swaps the two values passed as var-parameters. *)
PROCEDURE Swap(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b; 
    b := t
END Swap;
    
(* Applies the bubble-sort algorithm to the array. *)
PROCEDURE BubbleSort;
VAR i, j: INTEGER;
BEGIN
    i := 0;
    WHILE i < Dim DO
        j := Dim - 1;
        WHILE j > i DO
            IF a[j-1] > a[j] THEN Swap(a[j - 1], a[j]) END;
            j := j - 1
        END;
        i := i + 1
    END
END BubbleSort;

(* Applies the quick-sort algorithm to the array. *)
PROCEDURE QuickSort;

    PROCEDURE QSort(l, r: INTEGER);
    VAR i, j, x: INTEGER;
    BEGIN
        i := l;
        j := r;
        x := a[(r + l) DIV 2];
        WHILE i <= j DO
            WHILE a[i] < x DO i := i + 1 END;
            WHILE x < a[j] DO j := j - 1 END;
            IF i <= j THEN
                Swap(a[i], a[j]);
                i := i + 1;
                j := j - 1
            END
        END;
        IF l < j THEN QSort(l, j) END;
        IF i < r THEN QSort(i, r) END
    END QSort;

BEGIN
    QSort(0, Dim - 1)
END QuickSort;

(* Main program. *)
BEGIN
    Init;
    i := 1;
    REPEAT
        QuickSort();
        i := i + 1
    UNTIL i > Dim DIV 2


    (* i := 1;
    Write("[");
    WHILE i <= Dim DO
        Write(a[i]);
        IF i + 1 < Dim THEN Write(", ") END
    END;
    Write("]");
    WriteLn() *)
END Sort.