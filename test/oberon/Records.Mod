MODULE Records;

(* TYPE TParent = RECORD var1, param: INTEGER END;
     TChild = RECORD var2, param: INTEGER; parent: TParent END; *)

CONST Dim = 10;
TYPE TArray = ARRAY Dim OF INTEGER;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE print(a: TArray);
VAR i: INTEGER;
BEGIN
    printf("[");
    FOR i := 0 TO Dim - 1 DO
        IF i < Dim - 1 THEN
            printf("%d, ", a[i])
        ELSE
            printf("%d", a[i])
        END
    END;
    printf("]\n")
END print;

(* PROCEDURE Child2(param: INTEGER; VAR parent: TChild);
BEGIN
    parent.var2 := param;
    parent.parent.var1 := param
END Child2;

PROCEDURE Child1(param: INTEGER; VAR parent: TParent);
VAR var2: INTEGER;
    this: TChild;
BEGIN
    this.param := param;
    this.var2 := 0;
    this.parent := parent;
    Child2(this.param, this);
    printf("Child1@var1: %d\n", this.parent.var1);
    printf("Child1@var2: %d\n", this.var2);
    parent := this.parent
END Child1;

PROCEDURE Parent(param: INTEGER);
VAR var1: INTEGER;
    this: TParent;
BEGIN
    this.param := param;
    this.var1 := 0;
    Child1(this.param, this);
    printf("Parent@var1: %d\n", this.var1)
END Parent; *)

PROCEDURE Assignment;
TYPE TRecord = RECORD x, y, z: INTEGER END;
VAR r1, r2: TRecord;
BEGIN
    r1.x := 1; r1.y := 2; r1.z := 3;
    r2 := r1;
    r1.x := 4; r1.y := 5; r1.z := 6;
    (* printf("{ %d, %d, %d }\n", r2.x, r2.y, r2.z);
    r2.x := 4; r2.y := 5; r2.z := 6; *)
    printf("{ %d, %d, %d }\n", r1.x, r1.y, r1.z);
    printf("{ %d, %d, %d }\n", r2.x, r2.y, r2.z)
END Assignment;

PROCEDURE ArrayAssignment;
VAR a1, a2: TArray;
    i: INTEGER;
BEGIN
    FOR i := 0 TO Dim - 1 DO
        a1[i] := i
    END;
    print(a1);
    a2 := a1;
    print(a2);
    FOR i := 0 TO Dim - 1 DO
        a1[i] := Dim - i
    END;
    print(a1);
    print(a2)
END ArrayAssignment;

BEGIN
    (* Parent(100); *)
    ArrayAssignment
END Records.