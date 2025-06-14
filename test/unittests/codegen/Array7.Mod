(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Array7;
IMPORT Out;

PROCEDURE Test;
VAR
  i : INTEGER;
  a : ARRAY 3 OF CHAR;
BEGIN
  FOR i := 0 TO 2 DO
    a[i] := CHR(ORD("a") + i)
  END;
  Out.Char(a[0]); Out.Ln;
  Out.Char(a[1]); Out.Ln;
  Out.Char(a[2]); Out.Ln
END Test;

BEGIN
    Test
END Array7.
(*
    CHECK: a
    CHECK: b
    CHECK: c
*)