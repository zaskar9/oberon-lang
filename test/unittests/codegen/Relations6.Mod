(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Segmentation fault
*)
MODULE Relations6;

IMPORT Out;

PROCEDURE Test;
BEGIN
  IF 0AX = 0AX THEN
    IF 0FX > 0AX THEN
      IF 0AX < 0FX THEN
        Out.String("PASS"); Out.Ln;
        RETURN
      END
    END
  END;
  Out.String("FAIL"); Out.Ln
END Test;

BEGIN
    Test
END Relations6.
(*
    CHECK-NOT: FAIL
    CHECK: PASS
*)