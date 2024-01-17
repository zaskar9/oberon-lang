(*
  RUN: %oberon --run %s
  XFAIL: *
  UNSUPPORTED: *
  Should give error or warning? that procedure return not handled
*)
MODULE ProcedureReturnNotHandled;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test : INTEGER;
BEGIN
  RETURN 123
END Test;

BEGIN
    Test
END ProcedureReturnNotHandled.