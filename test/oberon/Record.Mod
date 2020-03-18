MODULE Record;

TYPE TParent = RECORD var1, param: INTEGER END;
     TChild = RECORD var2, param: INTEGER; parent: TParent END;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Child2(param: INTEGER; VAR parent: TChild);
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
END Parent;

BEGIN
    Parent(100)
END Record.