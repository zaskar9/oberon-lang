(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE For1;

IMPORT Out;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
    FOR x := 0 TO 3 DO Out.Int(x, 0); Out.Ln END
END Test;

BEGIN
    Test
END For1.
(*
    CHECK: 0
    CHECK: 1
    CHECK: 2
    CHECK: 3
*)