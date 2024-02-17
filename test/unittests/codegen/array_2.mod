(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Array2;

IMPORT Out;

PROCEDURE Test;
VAR
  i : INTEGER;
  a : ARRAY 3 OF REAL;
BEGIN
  FOR i := 0 TO 2 DO
    a[i] := i + 1.5
  END;
  Out.Real(a[0]); Out.Ln;
  Out.Real(a[1]); Out.Ln;
  Out.Real(a[2]); Out.Ln
END Test;

BEGIN
    Test
END Array2.
(*
    CHECK: 1.5
    CHECK: 2.5
    CHECK: 3.5
*)