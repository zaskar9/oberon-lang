MODULE VarParams;

TYPE List = ARRAY 2 OF INTEGER;

VAR a: List;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Swap(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b;
    b := t
END Swap;

PROCEDURE one(VAR a: List);
BEGIN
    Swap(a[0], a[1])
END one;

PROCEDURE two(VAR a: List);
BEGIN
    one(a)
END two;

BEGIN
    a[0] := 42; a[1] := 24;
    one(a);
    printf("[%d,%d]\n", a[0], a[1]);
    two(a);
    printf("[%d,%d]\n", a[0], a[1])
END VarParams.