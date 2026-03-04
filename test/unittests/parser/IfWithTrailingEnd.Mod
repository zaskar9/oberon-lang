(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
  Error message could be improved to complain about END in first IF statement.
*)
MODULE IfWithTrailingEnd;

VAR
  ret : INTEGER;

PROCEDURE Test(): INTEGER;
BEGIN
  IF TRUE THEN RETURN 123 END
  ELSIF FALSE THEN RETURN 213
  ELSE RETURN 321 END
END Test;

BEGIN
    ret := Test()
END IfWithTrailingEnd.
(*
  CHECK: {{.*}}:13:3:{{.*}}error:{{.*}}END expected, found ELSIF.
*)