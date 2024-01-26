(*
  RUN: %oberon --run %s
  XFAIL: *
  UNSUPPORTED: *
  Should trigger UBSan routine __ubsan_handle_out_of_bounds
*)
MODULE Array6;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

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