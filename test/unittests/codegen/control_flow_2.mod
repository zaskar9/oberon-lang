(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE ControlFlow2;

IMPORT Out;

PROCEDURE Test : INTEGER;
BEGIN
  IF TRUE THEN RETURN 123
  ELSE RETURN 321 END
END Test;

BEGIN
    Out.Int(Test(), 0); Out.Ln
END ControlFlow2.
(*
    CHECK: 123
*)