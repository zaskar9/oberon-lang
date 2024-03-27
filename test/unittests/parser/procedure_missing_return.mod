(*
  RUN: %oberon --run %s
  XFAIL: *
  REQUIRES: revision
  Does not complain about `RETURN` not reachable and missing
*)
MODULE ProcedureMissingReturn;

VAR
  ret : INTEGER;

PROCEDURE Test(): INTEGER;
BEGIN
  IF FALSE THEN RETURN 123 END
END Test;

BEGIN
   ret := Test()
END ProcedureMissingReturn.