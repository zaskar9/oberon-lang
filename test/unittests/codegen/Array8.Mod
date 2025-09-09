(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE Array8;
IMPORT Out;

TYPE Vector = ARRAY 10 OF INTEGER;
     Matrix = ARRAY 10, 10 OF INTEGER;

VAR v: Vector;
    m: Matrix;

(* --- Open array assignments --- *)

PROCEDURE Assign1(VAR dst, src: ARRAY OF INTEGER);
BEGIN
    dst := src
END Assign1;

PROCEDURE Assign2(VAR m: ARRAY OF ARRAY OF INTEGER; v: ARRAY OF INTEGER; i: INTEGER);
BEGIN
    m[i] := v
END Assign2;

PROCEDURE Assign3(VAR v: ARRAY OF INTEGER; m: ARRAY OF ARRAY OF INTEGER; i: INTEGER);
BEGIN
    v := m[i]
END Assign3;

(* --- Initialization --- *)

PROCEDURE Init1D(VAR a: ARRAY OF INTEGER; val: INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(a) - 1) DO a[i] := val + i END
END Init1D;

PROCEDURE Init2D(VAR a: ARRAY OF ARRAY OF INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(a) - 1) DO
        Init1D(a[i], i * SHORT(LEN(a, 1)))
    END
END Init2D;

(* --- Output --- *)

PROCEDURE Print1D(VAR v: ARRAY OF INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(v) - 1) DO Out.Int(v[i], 4) END
END Print1D;

PROCEDURE Print2D(VAR m: ARRAY OF ARRAY OF INTEGER);
VAR i: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(m) - 1) DO Print1D(m[i]); Out.Ln END
END Print2D;

(* --- Tests --- *)

PROCEDURE Test1();
VAR m: Matrix;
    v: Vector;
BEGIN
    Init2D(m);
    ASSERT(m[0, 0] = 0);
    ASSERT(m[9, 9] = 99);
    Init1D(v, 100);
    ASSERT(v[0] = 100);
    ASSERT(v[9] = 109);
    Assign1(m[0], v);
    ASSERT(m[0, 0] = v[0]);
    ASSERT(m[0, 9] = v[9]);
    Init1D(v, 110);
    ASSERT(v[0] = 110);
    ASSERT(v[9] = 119);
    Assign2(m, v, 9);
    ASSERT(m[9, 0] = v[0]);
    ASSERT(m[9, 9] = v[9]);
    (* Print2D(m); *)
    Assign3(v, m, 5);
    ASSERT(v[0] = m[5, 0]);
    ASSERT(v[9] = m[5, 9])
    (* Print1D(v); Out.Ln *)
END Test1;

PROCEDURE Test2();
VAR m: Matrix;
    v: Vector;
BEGIN
    Init2D(m);
    ASSERT(m[0, 0] = 0);
    ASSERT(m[9, 9] = 99);
    Init1D(v, 100);
    ASSERT(v[0] = 100);
    ASSERT(v[9] = 109);
    m[0] := v;
    ASSERT(m[0, 0] = v[0]);
    ASSERT(m[0, 9] = v[9]);
    (* Print2D(m); *)
    Init1D(v, 110);
    ASSERT(v[0] = 110);
    ASSERT(v[9] = 119);
    v := m[5];
    ASSERT(v[0] = m[5, 0]);
    ASSERT(v[9] = m[5, 9])
    (* Print1D(v); Out.Ln *)
END Test2;

BEGIN
    Test1;
    Test2;
    Init2D(m);
    ASSERT(m[0, 0] = 0);
    ASSERT(m[9, 9] = 99);
    Init1D(v, 100);
    ASSERT(v[0] = 100);
    ASSERT(v[9] = 109);
    Assign1(m[0], v);
    ASSERT(m[0, 0] = v[0]);
    ASSERT(m[0, 9] = v[9]);
    Init1D(v, 110);
    ASSERT(v[0] = 110);
    ASSERT(v[9] = 119);
    Assign2(m, v, 9);
    ASSERT(m[9, 0] = v[0]);
    ASSERT(m[9, 9] = v[9]);
    (* Print2D(m); *)
    Assign3(v, m, 5);
    ASSERT(v[0] = m[5, 0]);
    ASSERT(v[9] = m[5, 9])
    (* Print1D(v); Out.Ln *)
END Array8.