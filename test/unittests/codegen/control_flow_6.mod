(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE ControlFlow6;

IMPORT Out;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
  x := 2;
  IF x = 2 THEN
    IF x = 3 THEN
      RETURN
    END
  ELSIF x = 3 THEN
    RETURN
  ELSE
    RETURN
  END;
  Out.String("PASS"); Out.Ln
END Test;

BEGIN
    Test
END ControlFlow6.
(*
    CHECK: PASS
*)