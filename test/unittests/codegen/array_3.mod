(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Array3;

IMPORT Out;

PROCEDURE Test;
VAR
  a : ARRAY 3 OF BOOLEAN;
  b : BOOLEAN;
  i : INTEGER;
BEGIN
  b := TRUE;
  FOR i := 0 TO 2 DO
    a[i] := b;
    b := ~b
  END;
  FOR i := 0 TO 2 DO
    IF a[i] THEN Out.Int(1, 0); Out.Ln
    ELSE Out.Int(0, 0); Out.Ln END
  END
END Test;

BEGIN
    Test
END Array3.
(*
    CHECK: 1
    CHECK: 0
    CHECK: 1
*)