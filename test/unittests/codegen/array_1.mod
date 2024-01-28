(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Array1;

IMPORT Out;

PROCEDURE Test;
VAR
  i : INTEGER;
  a : ARRAY 3 OF INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i] := i + 1
  END;
  Out.Int(a[0], 0); Out.Ln;
  Out.Int(a[1], 0); Out.Ln;
  Out.Int(a[2], 0); Out.Ln
END Test;

BEGIN
    Test
END Array1.
(*
    CHECK: 1
    CHECK: 2
    CHECK: 3
*)