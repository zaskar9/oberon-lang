(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Array4;

IMPORT Out;

PROCEDURE Test;
VAR
  a : ARRAY 3 OF RECORD x : INTEGER END;
  i : INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i].x := i
  END;
  Out.Int(a[0].x, 0); Out.Ln;
  Out.Int(a[1].x, 0); Out.Ln;
  Out.Int(a[2].x, 0); Out.Ln
END Test;

BEGIN
    Test
END Array4.
(*
    CHECK: 0
    CHECK: 1
    CHECK: 2
*)