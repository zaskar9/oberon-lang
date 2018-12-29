(* Implementation of the Bubble Sort algorithm. *)
MODULE BubbleSort;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;

TYPE Point2D = RECORD x, y: INTEGER END;
     INT = INTEGER;

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INT;
    b: Point2D;

PROCEDURE Test();
VAR point: Point2D;
    i: INTEGER;
BEGIN
    point.x := 1;
    point.y := -1;
    WHILE 1 # 1 DO
        i := Dim
    END
END Test;

(* Initializes the array. *)
PROCEDURE Init();
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE i < Dim DO
        a[i] := Dim-i;
        i := i + 1
    END
END Init;

(* Swaps the two values passed as var-parameters. *)
PROCEDURE Swap(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b; 
    b := t
END Swap;
    
(* Applies the bubble sort algorithm to the array. *)
PROCEDURE Bubble;
VAR i, j: INTEGER;
BEGIN
    i := 1;
    WHILE i <= Dim DO
        j := Dim; 
        WHILE j > i DO
            IF a[j-1] > a[j] THEN Swap(a[j - 1], a[j]) END;
            j := j - 1
        END;
        i := i + 1
    END
END Bubble;

(* Main program. *)
BEGIN
    Init;
    Bubble()
END BubbleSort.