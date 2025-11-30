(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE TrapCopyOverflow2;
IMPORT Out;

TYPE Vector = ARRAY 10 OF INTEGER;
     List = ARRAY 20 OF INTEGER;

VAR v: Vector;
    l: List;
    i: INTEGER;

PROCEDURE assign(VAR v: Vector; a: ARRAY OF INTEGER);
BEGIN
    v := a
END assign;

BEGIN
    FOR i := 0 TO 19 DO
        l[i] := i
    END;
    assign(v, l);
    FOR i := 0 TO 9 DO
        Out.Int(v[i], 0); Out.Char(" ")
    END;
    Out.Ln
END TrapCopyOverflow2.
(*
  CHECK: {{.*}}code 3 (array or string copy overflow)
*)