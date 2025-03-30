(*
  RUN: %oberon --run %s
  XFAIL: *
  Return -1.#INF, but should trigger UBSAN routine __ubsan_handle_divrem_overflow.
*)
MODULE Arithmetic15;

PROCEDURE Test;
VAR 
  a, b, c : REAL;
BEGIN
  a := -7.5;
  b := 0.0;
  c := a / b
END Test;

BEGIN
    Test
END Arithmetic15.