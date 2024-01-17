(*
  RUN: %oberon --run %s
  XFAIL: *
  UNSUPPORTED: *
  Note does not complain about return not reachable and missing
*)
MODULE ProcedureMissingReturn;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test : INTEGER;
BEGIN
  IF FALSE THEN RETURN 123 END
END Test;

BEGIN
    printf("%d", Test())
END ProcedureMissingReturn.