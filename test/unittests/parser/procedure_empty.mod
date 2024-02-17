(*
  RUN: %oberon --run %s
  It seems an empty procedure is not supported.
*)
MODULE ProcedureEmpty;

PROCEDURE Test;
BEGIN END Test;

BEGIN
    Test()
END ProcedureEmpty.