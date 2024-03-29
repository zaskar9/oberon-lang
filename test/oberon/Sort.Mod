(* Oberon implementations of various sorting algorithms. *)
MODULE Sort;
IMPORT Oberon, Out, Random;

(* Length of the array to be sorted. *)
CONST Dim* = 5 * 0AH;
      MaxVal = 10 * Dim;

(* Array to be sorted. *)
TYPE Vector* = ARRAY Dim OF INTEGER;

(* Array to be sorted. *)
VAR list*: Vector;
    start: LONGINT;

(* Initializes the array. *)
PROCEDURE Init(VAR a: Vector);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO Dim -1 BY 1 DO
        a[i] := Random.Int(MaxVal)
    END
END Init;

(* Prints the array. *)
PROCEDURE Print(VAR a: Vector);
VAR i: INTEGER;
BEGIN
    Out.String("[");
    i := 0;
    WHILE i < Dim DO
        Out.Int(a[i], 0);
        IF i + 1 < Dim THEN Out.String(",") END;
        i := i + 1
    END;
    Out.String("]");
    Out.Ln
END Print;

(* Swaps the two values passed as var-parameters. *)
PROCEDURE Swap(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b;
    b := t
END Swap;

PROCEDURE IsSorted(a: Vector): BOOLEAN;
VAR i, current: INTEGER;
BEGIN
    current := a[0];
    FOR i := 1 TO SHORT(LEN(a) - 1) DO
        IF current > a[i] THEN
            RETURN FALSE
        ELSE
            current := a[i]
        END
    END;
    RETURN TRUE
END IsSorted;

(* ------------------------------------------------- InsertionSort ------------------------------------------------ *)
PROCEDURE InsertionSort(VAR a: Vector);
VAR i, j: INTEGER;
BEGIN
    i := 1;
    WHILE i < Dim DO
        j := i;
        WHILE (j > 0) & (a[j - 1] > a[j]) DO
            Swap(a[j], a[j - 1]);
            DEC(j)
        END;
        INC(i)
    END
END InsertionSort;

(* ------------------------------------------------- SelectionSort ------------------------------------------------ *)
PROCEDURE SelectionSort(VAR a: Vector);
VAR i, j: INTEGER;
    min: INTEGER;
BEGIN
    i := 0;
    WHILE i < Dim - 1 DO
        min := i;
        j := i + 1;
        WHILE  j < Dim DO
            IF a[j] < a[min] THEN min := j END;
            INC(j)
        END;
        IF min # i THEN Swap(a[i], a[min]) END;
        INC(i)
    END
END SelectionSort;

(* -------------------------------------------------- BubbleSort -------------------------------------------------- *)
PROCEDURE BubbleSort(VAR a: Vector);
VAR i, j: INTEGER;
BEGIN
    i := 0;
    WHILE i < Dim DO
        j := Dim - 1;
        WHILE j > i DO
            IF a[j - 1] > a[j] THEN Swap(a[j - 1], a[j]) END;
            DEC(j)
        END;
        INC(i)
    END
END BubbleSort;

(* -------------------------------------------------- QuickSort --------------------------------------------------- *)
PROCEDURE QuickSort(VAR a: Vector);

    PROCEDURE QSort(VAR a: Vector; l, r: INTEGER);
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
                INC(i);
                DEC(j)
            END
        END;
        IF l < j THEN QSort(a, l, j) END;
        IF i < r THEN QSort(a, i, r) END
    END QSort;

BEGIN
    QSort(a, 0, Dim - 1)
END QuickSort;

(* -------------------------------------------------- MergeSort --------------------------------------------------- *)
PROCEDURE MergeSort(VAR a: Vector);

    PROCEDURE MSort(VAR a: Vector; l, r: INTEGER);
    VAR
        m: INTEGER;
        h, i, j, k: INTEGER;
        tmp: ARRAY Dim OF INTEGER;                              (* helper array to merge the two parts              *)
    BEGIN
        IF l < r THEN                                           (* otherwise: one element or empty sequence         *)
            m := (l + r) DIV 2;                                 (* m points to the middle of the sequence           *)
            MSort(a, l, m);                                     (* sort lower half recursively                      *)
            MSort(a, m + 1, r);                                 (* sort upper half recursively                      *)
            i := l;
            j := m + 1;
            k := l;                                             (* counter variable to index the helper array       *)
            WHILE (i < m + 1) & (j <= r) DO                     (* both parts have more elements                    *)
                IF a[i] < a[j] THEN                             (* choose smaller element to copy into helper array *)
                    tmp[k] := a[i];
                    INC(i)
                ELSE
                    tmp[k] := a[j];
                    INC(j)
                END;
                INC(k)                                          (* move to next element in helper array             *)
            END;
            IF m < i THEN
                FOR h := j TO r DO tmp[k + h - j] := a[h] END   (* first part exhausted, copy second part           *)
            ELSE
                FOR h := i TO m DO tmp[k + h - i] := a[h] END   (* second part exhausted, copy first part           *)
            END;
            FOR h := l TO r DO a[h] := tmp[h] END               (* write helper array back to main array            *)
        END
    END MSort;

BEGIN
    MSort(a, 0, Dim - 1)
END MergeSort;

(* -------------------------------------------------- Main Program ------------------------------------------------ *)
BEGIN
    start := Oberon.TimeMicros();
    Random.Randomize;
    Out.String("--- Bubble Sort"); Out.Ln;
    Init(list);
    Print(list);
    BubbleSort(list);
    Print(list);
    ASSERT(IsSorted(list));
    Out.String("--- Insertion Sort"); Out.Ln;
    Init(list);
    Print(list);
    InsertionSort(list);
    Print(list);
    ASSERT(IsSorted(list));
    Out.String("--- Selection Sort"); Out.Ln;
    Init(list);
    Print(list);
    SelectionSort(list);
    Print(list);
    ASSERT(IsSorted(list));
    Out.String("--- Merge Sort"); Out.Ln;
    Init(list);
    Print(list);
    MergeSort(list);
    Print(list);
    ASSERT(IsSorted(list));
    Out.String("--- Quick Sort"); Out.Ln;
    Init(list);
    Print(list);
    QuickSort(list);
    Print(list);
    ASSERT(IsSorted(list));
    Out.String("--- Elapsed Time: "); Out.Long(Oberon.TimeMicros() - start, 0); Out.String(" μs."); Out.Ln
END Sort.
