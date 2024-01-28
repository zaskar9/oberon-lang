(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
  Print "NOPASS" first which should not be reached
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