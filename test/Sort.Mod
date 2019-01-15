(* Oberon-0 implementation of various sorting algorithms. *)
MODULE Sort;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;

TYPE Vector = ARRAY Dim OF INTEGER;

(* Array to be sorted. *)
VAR a: Vector;
    i: INTEGER;

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

(* Prints the array. *)
PROCEDURE Print();
VAR i: INTEGER;
BEGIN
    (* Write("["); *)
    WHILE i <= Dim DO
        (* Write(a[i]); *)
        (* IF i + 1 < Dim THEN Write(", ") END *)
        i = i + 1
    END
    (* Write("]"); *)
    (* WriteLn() *)
END Print;

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
    QuickSort();
    Print()
END Sort.
