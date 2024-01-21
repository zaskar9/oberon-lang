(* This is a generated file. Do not edit. *)

MODULE UnnestedSort(*0*);

   CONST Dim(*1*) = 50;
         MaxVal(*1*) = 10 * Dim;

   TYPE Vector(*1*) = ARRAY Dim OF INTEGER;
        TQuickSort(*1*) = RECORD a: Vector END;
        TMergeSort(*1*) = RECORD a: Vector END;

   VAR list(*1*): Vector;

   PROCEDURE printf(*1*)(format(*2*): STRING; ...): INTEGER; EXTERN;

   PROCEDURE rand(*1*)(): INTEGER; EXTERN;

   PROCEDURE Write(*1*)(str(*2*): STRING);
   BEGIN
      printf(str)
   END Write;

   PROCEDURE WriteLn(*1*)();
   BEGIN
      printf("\n")
   END WriteLn;

   PROCEDURE WriteInt(*1*)(val(*2*): INTEGER);
   BEGIN
      printf("%d", val)
   END WriteInt;

   PROCEDURE Init(*1*)(VAR a(*2*): Vector);
      VAR i(*2*): INTEGER;
   BEGIN
      FOR i := 0 TO Dim - 1 BY 1 DO
         a[i] := rand() MOD MaxVal
      END
   END Init;

   PROCEDURE Print(*1*)(VAR a(*2*): Vector);
      VAR i(*2*): INTEGER;
   BEGIN
      Write("[");
      i := 0;
      WHILE i < Dim DO
         WriteInt(a[i]);
         IF i + 1 < Dim THEN
            Write(",")
         END;
         i := i + 1
      END;
      Write("]");
      WriteLn()
   END Print;

   PROCEDURE Swap(*1*)(VAR a(*2*): INTEGER; VAR b(*2*): INTEGER);
      VAR t(*2*): INTEGER;
   BEGIN
      t := a;
      a := b;
      b := t
   END Swap;

   PROCEDURE InsertionSort(*1*)(VAR a(*2*): Vector);
      VAR i(*2*): INTEGER;
          j(*2*): INTEGER;
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

   PROCEDURE SelectionSort(*1*)(VAR a(*2*): Vector);
      VAR i(*2*): INTEGER;
          j(*2*): INTEGER;
          min(*2*): INTEGER;
   BEGIN
      i := 0;
      WHILE i < Dim - 1 DO
         min := i;
         j := i + 1;
         WHILE j < Dim DO
            IF a[j] < a[min] THEN
               min := j
            END;
            INC(j)
         END;
         IF min # i THEN
            Swap(a[i], a[min])
         END;
         INC(i)
      END
   END SelectionSort;

   PROCEDURE BubbleSort(*1*)(VAR a(*2*): Vector);
      VAR i(*2*): INTEGER;
          j(*2*): INTEGER;
   BEGIN
      i := 0;
      WHILE i < Dim DO
         j := Dim - 1;
         WHILE j > i DO
            IF a[j - 1] > a[j] THEN
               Swap(a[j - 1], a[j])
            END;
            DEC(j)
         END;
         INC(i)
      END
   END BubbleSort;

   PROCEDURE QSort(*1*)(VAR a(*2*): Vector; l(*2*): INTEGER; r(*2*): INTEGER; VAR parent(*2*): TQuickSort);
      VAR i(*2*): INTEGER;
          j(*2*): INTEGER;
          x(*2*): INTEGER;
   BEGIN
      i := l;
      j := r;
      x := a[(r + l) DIV 2];
      WHILE i <= j DO
         WHILE a[i] < x DO
            i := i + 1
         END;
         WHILE x < a[j] DO
            j := j - 1
         END;
         IF i <= j THEN
            Swap(a[i], a[j]);
            INC(i);
            DEC(j)
         END
      END;
      IF l < j THEN
         QSort(a, l, j, parent)
      END;
      IF i < r THEN
         QSort(a, i, r, parent)
      END
   END QSort;

   PROCEDURE MSort(*1*)(VAR a(*2*): Vector; l(*2*): INTEGER; r(*2*): INTEGER; VAR parent(*2*): TMergeSort);
      VAR m(*2*): INTEGER;
          h(*2*): INTEGER;
          i(*2*): INTEGER;
          j(*2*): INTEGER;
          k(*2*): INTEGER;
          tmp(*2*): ARRAY Dim OF INTEGER;
   BEGIN
      IF l < r THEN
         m := (l + r) DIV 2;
         MSort(a, l, m, parent);
         MSort(a, m + 1, r, parent);
         i := l;
         j := m + 1;
         k := l;
         WHILE (i < m + 1) & (j <= r) DO
            IF a[i] < a[j] THEN
               tmp[k] := a[i];
               INC(i)
            ELSE
               tmp[k] := a[j];
               INC(j)
            END;
            INC(k)
         END;
         IF m < i THEN
            FOR h := j TO r BY 1 DO
               tmp[k + h - j] := a[h]
            END
         ELSE
            FOR h := i TO m BY 1 DO
               tmp[k + h - i] := a[h]
            END
         END;
         FOR h := l TO r BY 1 DO
            a[h] := tmp[h]
         END
      END
   END MSort;

   PROCEDURE QuickSort(*1*)(VAR a(*2*): Vector);
      VAR this(*2*): TQuickSort;
   BEGIN
      this.a := a;
      QSort(this.a, 0, Dim - 1, this);
      a := this.a
   END QuickSort;

   PROCEDURE MergeSort(*1*)(VAR a(*2*): Vector);
      VAR this(*2*): TMergeSort;
   BEGIN
      this.a := a;
      MSort(this.a, 0, Dim - 1, this);
      a := this.a
   END MergeSort;

BEGIN
   Write("--- Bubble Sort");
   WriteLn();
   Init(list);
   Print(list);
   BubbleSort(list);
   Print(list);
   Write("--- Insertion Sort");
   WriteLn();
   Init(list);
   Print(list);
   InsertionSort(list);
   Print(list);
   Write("--- Selection Sort");
   WriteLn();
   Init(list);
   Print(list);
   SelectionSort(list);
   Print(list);
   Write("--- Merge Sort");
   WriteLn();
   Init(list);
   Print(list);
   MergeSort(list);
   Print(list);
   Write("--- Quick Sort");
   WriteLn();
   Init(list);
   Print(list);
   QuickSort(list);
   Print(list)
END UnnestedSort.
