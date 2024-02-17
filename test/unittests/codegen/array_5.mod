(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
  Fails with Segmentation fault
*)
MODULE Array5;

IMPORT Out;

PROCEDURE P;
BEGIN Out.Int(1, 0); Out.Ln END P;

PROCEDURE Test;
VAR
  a : ARRAY 3 OF RECORD p : PROCEDURE END;
  i : INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i].p := P
  END;
  FOR i := 0 TO 2 DO
    a[i].p()
  END
END Test;

BEGIN
    Test
END Array4.
(*
    CHECK: 1
    CHECK: 1
    CHECK: 1
*)