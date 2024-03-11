(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Prints "NOPASS" first, which should not be reached
*)
MODULE Loop1;

IMPORT Out;

PROCEDURE Test;
BEGIN
  LOOP
    RETURN
  END;
  Out.String("NOPASS")
END Test;

BEGIN
    Test;
    Out.String("PASS")
END Loop1.
(*
    CHECK-NOT: NOPASS
*)