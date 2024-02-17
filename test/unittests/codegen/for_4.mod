(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE For4;

IMPORT Out;

PROCEDURE Test;
VAR x, y : INTEGER;
BEGIN
    FOR x := 6 TO -1 BY -2 DO Out.Int(x, 0); Out.Ln END
END Test;

BEGIN
    Test
END For4.
(*
    CHECK: 6
    CHECK: 4
    CHECK: 2
    CHECK: 0
*)