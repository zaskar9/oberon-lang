(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE ControlFlow1;

IMPORT Out;

PROCEDURE Test : INTEGER;
BEGIN
  IF TRUE THEN RETURN 123 END;
  RETURN 321
END Test;

BEGIN
    Out.Int(Test(), 0); Out.Ln
END ControlFlow1.
(*
    CHECK: 123
*)