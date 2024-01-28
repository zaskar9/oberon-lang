(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE For3;

IMPORT Out;

PROCEDURE Test;
VAR x, y : INTEGER;
BEGIN
    y := 7;
    FOR x := 0 TO y BY 2 DO Out.Int(x, 0); Out.Ln END
END Test;

BEGIN
    Test
END For3.
(*
    CHECK: 0
    CHECK: 2
    CHECK: 4
    CHECK: 6
*)