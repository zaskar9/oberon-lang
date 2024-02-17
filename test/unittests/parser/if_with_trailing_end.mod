(*
  RUN: %oberon --run %s
  XFAIL: *
  Note does not complain about end in first IF statment
*)
MODULE IfWithTrailingEnd;

VAR
  ret : INTEGER;

PROCEDURE Test : INTEGER;
BEGIN
  IF TRUE THEN RETURN 123 END
  ELSIF FALSE THEN RETURN 213
  ELSE RETURN 321 END
END Test;

BEGIN
    ret := Test()
END IfWithTrailingEnd.