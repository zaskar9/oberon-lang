(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE ControlFlow3;

IMPORT Out;

PROCEDURE Test : INTEGER;
VAR x : INTEGER;
BEGIN
  x := 2;
  IF x = 2 THEN RETURN 123
  ELSIF x = 3 THEN RETURN 213
  ELSE RETURN 321 END
END Test;

BEGIN
    Out.Int(Test(), 0); Out.Ln
END ControlFlow3.
(*
    CHECK: 123
*)