(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Prints "NOPASS" first, which should not be reached
*)
MODULE Loop2;

IMPORT Out;

PROCEDURE Test;
VAR x: INTEGER;
BEGIN
  x := 0;
  LOOP
    IF x = 0 THEN EXIT END;
    Out.String("NOPASS"); Out.Ln
  END
END Test;

BEGIN
    Test;
    Out.String("PASS"); Out.Ln
END Loop2.
(*
    CHECK-NOT: NOPASS
*)