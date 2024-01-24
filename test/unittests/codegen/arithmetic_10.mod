(*
  RUN: %oberon --run %s
  XFAIL: *
  UNSUPPORTED: *
  Crash, but should trigger UBSAN routine __ubsan_handle_divrem_overflow.
*)
MODULE Arithmetic10;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  a := 1; b := 0;
  c := a DIV b
END Test;

BEGIN
    Test
END Arithmetic10.