(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE System2;

IMPORT SYSTEM, Out;

PROCEDURE Test;
VAR 
    xadr, yadr : LONGINT;
    x, y : ARRAY 4 OF INTEGER;
    i : INTEGER;
BEGIN
    FOR i := 0 TO 3 DO x[i] := (i + 1) END;
    FOR i := 0 TO 3 DO y[i] := -(i + 1) END;
    xadr := SYSTEM.ADR(x[0]);
    yadr := SYSTEM.ADR(y[0]);
    SYSTEM.COPY(xadr, yadr, LEN(x));
    FOR i := 0 TO 3 DO Out.Int(y[i], 0); Out.Ln END
END Test;

BEGIN
    Test
END System2.
(*
    CHECK: 1
    CHECK: 2
    CHECK: 3
    CHECK: 4
*)