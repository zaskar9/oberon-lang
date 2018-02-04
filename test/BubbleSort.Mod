MODULE BubbleSort;

CONST Dim = 20;
VAR a: ARRAY Dim OF INTEGER;

PROCEDURE Init();
VAR i: INTEGER;
BEGIN
	i := 0;
	WHILE i < Dim DO
		a[i] := Dim-i;
		i := i + 1
	END
END Init;

PROCEDURE Swap(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
	t := a;
	a := b; 
	b := t
END Swap;
	

PROCEDURE Bubble;
VAR i, j : INTEGER;
BEGIN
	i := Dim;
	WHILE i > 1 DO
		j := i-1; 
		WHILE j > 0 DO
			IF a[i] > a[j] THEN Swap(a[i], a[j]) END;
			j := j + 1
		END;
		i := i + 1
	END
END Bubble;

BEGIN
	Init;
	Bubble()
END BubbleSort.