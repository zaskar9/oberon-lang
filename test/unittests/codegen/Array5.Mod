(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Array5;
IMPORT Out;

CONST SUM = 1; PROD = 2;

TYPE ProcType1 = PROCEDURE;
     ProcType2 = PROCEDURE(): INTEGER;
     ProcType3 = PROCEDURE(x: INTEGER);
     ProcType4 = PROCEDURE(x: INTEGER): INTEGER;
     ProcType5 = PROCEDURE: INTEGER;
     ProcType6 = PROCEDURE(x, y: INTEGER; z: INTEGER): REAL;
     ProcType7 = PROCEDURE(x, y: INTEGER): INTEGER;
     ChooseType = PROCEDURE(op: INTEGER): ProcType7;
     ArrayRec1 = ARRAY 3 OF RECORD proc: PROCEDURE; foo: ProcType7; bar: ProcType5 END;
     ArrayRec2 = ARRAY 1 OF RECORD choose: ChooseType END;

VAR p: ProcType7;
    i: INTEGER;
    a1: ArrayRec1;
    a2: ArrayRec2;

PROCEDURE Proc;
BEGIN
    Out.Int(1, 0)
END Proc;

PROCEDURE One(): INTEGER;
BEGIN
    RETURN 1
END One;

PROCEDURE Sum(x, y: INTEGER): INTEGER;
BEGIN
    RETURN x + y
END Sum;

PROCEDURE Prod(x, y: INTEGER): INTEGER;
BEGIN
    RETURN x * y
END Prod;

PROCEDURE Test;
VAR a: ArrayRec1;
    i: INTEGER;
BEGIN
    FOR i := 0 TO 2 DO
        a[i].proc := Proc;
        a[i].foo := Sum;
        a[i].bar := One
    END;
    FOR i := 0 TO 2 DO
        a[i].proc(); Out.Ln;
        ASSERT(a[i].foo(i, i) = i + i);
        ASSERT(a[i].bar() = 1);
        a[i].bar()
    END
END Test;

PROCEDURE Apply(f: ProcType7; x, y: INTEGER): INTEGER;
BEGIN
    RETURN f(x, y)
END Apply;

PROCEDURE Choose(op: INTEGER): ProcType7;
VAR p: ProcType7;
BEGIN
    p := NIL;
    CASE op OF
        1: p := Sum
      | 2: p := Prod
    END;
    RETURN p
END Choose;

BEGIN
    Test;
    p := Sum;
    ASSERT(p(1, 2) = 3);
    ASSERT(Apply(Sum, 1, 2) = (3));
    ASSERT(Apply(Choose(SUM), 1, 2) = 3);
    ASSERT(Apply(Choose(PROD), 1, 2) = 2);
    p := Choose(PROD);
    ASSERT(p(2, 3) = 6);
    a1[0].foo := Choose(SUM);
    ASSERT(a1[0].foo(2, 3) = 5);
    ASSERT(Choose(PROD)(2, 3) = 6);
    a2[0].choose := Choose;
    ASSERT(a2[0].choose(PROD)(2, 3) = 6)
END Array5.
(*
    CHECK: 1
    CHECK: 1
    CHECK: 1
*)