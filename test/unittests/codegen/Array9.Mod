(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Array9;
IMPORT Out, SYSTEM;

CONST LenX = 2;
      LenY = 3;
      LenZ = 4;

TYPE Vector = ARRAY LenX OF INTEGER;
     Matrix = ARRAY LenY, LenX OF INTEGER;
     Cube = ARRAY LenZ, LenY, LenX OF INTEGER;

VAR m: Matrix;
    c: Cube;

PROCEDURE Init2D(VAR v: ARRAY OF ARRAY OF INTEGER);
VAR i, j: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(v, 0) - 1) DO
        FOR j := 0 TO SHORT(LEN(v, 1) - 1) DO
            v[i, j] := i * SHORT(LEN(v, 1)) + j
        END
    END
END Init2D;

PROCEDURE Init3D(VAR c: ARRAY OF ARRAY OF ARRAY OF INTEGER);
VAR i, j, k: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(c, 0) - 1) DO
        FOR j := 0 TO SHORT(LEN(c, 1) - 1) DO
            FOR k := 0 TO SHORT(LEN(c, 2) - 1) DO
                c[i, j, k] := i * SHORT(LEN(c, 1)) * SHORT(LEN(c, 2)) + j * SHORT(LEN(c, 2)) + k
            END
        END
    END
END Init3D;

PROCEDURE Dump2D(v: ARRAY OF ARRAY OF INTEGER);
TYPE List = ARRAY LenY * LenX OF INTEGER;
VAR l: List;
    i: INTEGER;
BEGIN
    l := SYSTEM.VAL(List, v);
    FOR i := 0 TO SHORT(LEN(l) - 1) DO
        Out.Int(l[i], 3)
    END
END Dump2D;

PROCEDURE Dump3D(c: Cube);
VAR i, j, k: INTEGER;
BEGIN
    FOR i := 0 TO SHORT(LEN(c, 0) - 1) DO
        FOR j := 0 TO SHORT(LEN(c, 1) - 1) DO
            FOR k := 0 TO SHORT(LEN(c, 2) - 1) DO
                Out.Int(c[i, j, k], 3)
            END
        END
    END
END Dump3D;

BEGIN
    Init2D(m);
    Dump2D(m);
    Out.Ln;
    Init3D(c);
    Dump3D(c)
END Array9.
(*
  CHECK:   0  1  2  3  4  5
  CHECK:   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
*)