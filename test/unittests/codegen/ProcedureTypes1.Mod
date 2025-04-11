(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE ProcedureTypes1;
IMPORT Out;

CONST Len = 10;

TYPE UnOp = PROCEDURE(x: INTEGER): INTEGER;
     BinOp = PROCEDURE(x, y: INTEGER): INTEGER;
     Array = ARRAY Len OF UnOp;
     Record = RECORD unary: UnOp; binary: BinOp END;
     ArrayRec = ARRAY Len OF Record;
     RecArray = RECORD array: Array END;

VAR i: INTEGER;
    globals: ARRAY Len OF INTEGER;

PROCEDURE Plus(x, y: INTEGER): INTEGER;
BEGIN
    RETURN x + y
END Plus;

PROCEDURE Times(x, y: INTEGER): INTEGER;
BEGIN
    RETURN x * y
END Times;

PROCEDURE Neg(x: INTEGER): INTEGER;
BEGIN
    RETURN -x
END Neg;

PROCEDURE Inc(x: INTEGER): INTEGER;
BEGIN
    RETURN x + 1
END Inc;

PROCEDURE Dec(x: INTEGER): INTEGER;
BEGIN
    RETURN x - 1
END Dec;

PROCEDURE Apply1(op: Record; x: INTEGER): INTEGER;
BEGIN
    RETURN op.unary(x)
END Apply1;

PROCEDURE Apply2(op: Record; x, y: INTEGER): INTEGER;
BEGIN
    RETURN op.binary(x, y)
END Apply2;

PROCEDURE Map1(ops: ArrayRec; VAR a: ARRAY OF INTEGER);
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE (i < LEN(ops)) & (i < LEN(a)) DO
        a[i] := ops[i].unary(a[i]);
        INC(i)
    END
END Map1;

PROCEDURE Fold2(op: Record; a: ARRAY OF INTEGER; x: INTEGER): INTEGER;
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(a)) - 1 DO
        x := op.binary(x, a[i])
    END;
    RETURN x
END Fold2;

PROCEDURE Sum(a: ARRAY OF INTEGER): INTEGER;
VAR plusrec: Record;
BEGIN
    plusrec.binary := Plus;
    RETURN Fold2(plusrec, a, 0)
END Sum;

PROCEDURE Test1;
VAR i: INTEGER;
    funs: ArrayRec;
    vals: ARRAY Len OF INTEGER;
    increc, decrec, negrec: Record;
BEGIN
    increc.unary := Inc;
    decrec.unary := Dec;
    negrec.unary := Neg;
    FOR i := 0 TO Len - 1 DO
        CASE i MOD 3 OF
           0: funs[i] := increc
         | 1: funs[i] := decrec
         | 2: funs[i] := negrec
        END;
        vals[i] := i
    END;
    Map1(funs, vals);
    Out.Int(Sum(vals), 0); Out.Ln
END Test1;

PROCEDURE Test2;
VAR i: INTEGER;
    vals: ARRAY Len OF INTEGER;
BEGIN
    FOR i := 0 TO Len - 1 DO vals[i] := i END;
    Out.Int(Sum(vals), 0); Out.Ln
END Test2;

BEGIN
    Test1;
    Test2
END ProcedureTypes1.
(*
  CHECK: 16
  CHECK: 45
*)