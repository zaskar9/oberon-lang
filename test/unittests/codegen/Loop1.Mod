(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  REQUIRES: revision
  Goes into a infinite loop on EXIT statment
*)
MODULE Loop1;

IMPORT Out;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
  x := 2;
  LOOP
    IF x = 2 THEN
      IF x = 3 THEN
        RETURN
      END
    ELSIF x = 3 THEN
      RETURN
    ELSE
      RETURN
    END;
    EXIT;
    x := 5
  END;
  Out.String("PASS"); Out.Ln
END Test;

BEGIN
    Test
END Loop1.
(*
    CHECK: PASS
*)