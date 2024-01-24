(*
  RUN: %oberon --run %s
  XFAIL: *
  UNSUPPORTED: *
  Note does not complain about end in first IF statment
*)
MODULE IfWithTrailingEnd;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test : INTEGER;
BEGIN
  IF TRUE THEN RETURN 123 END
  ELSIF FALSE THEN RETURN 213
  ELSE RETURN 321 END
END Test;

BEGIN
    printf("%d", Test())
END IfWithTrailingEnd.