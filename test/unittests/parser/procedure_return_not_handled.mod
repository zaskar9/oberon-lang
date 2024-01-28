(*
  RUN: %oberon --run %s
  XFAIL: *
  Should give error or warning? that procedure return not handled
*)
MODULE ProcedureReturnNotHandled;

PROCEDURE Test : INTEGER;
BEGIN
  RETURN 123
END Test;

BEGIN
    Test
END ProcedureReturnNotHandled.