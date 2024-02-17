(*
  RUN: %oberon --run %s
  XFAIL: *
  Does not complain about return not reachable and missing
*)
MODULE ProcedureMissingReturn;

VAR
  ret : INTEGER;

PROCEDURE Test : INTEGER;
BEGIN
  IF FALSE THEN RETURN 123 END
END Test;

BEGIN
   ret := Test()
END ProcedureMissingReturn.