(*
  RUN: %oberon --run %s
  XFAIL: *
  Should trigger UBSan routine __ubsan_handle_out_of_bounds
*)
MODULE Array6;

PROCEDURE Test;
VAR
  i : INTEGER;
  a : ARRAY 3 OF INTEGER;
BEGIN
  FOR i := 0 TO 5 DO
    a[i] := i + 1
  END
END Test;

BEGIN
    Test
END Array6.