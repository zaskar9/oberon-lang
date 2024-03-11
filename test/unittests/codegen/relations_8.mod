(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Segmentation fault
*)
MODULE Relations8;
IMPORT Out;

VAR ch1, ch2: CHAR;

PROCEDURE Test;
BEGIN
  IF ch1 = ch1 THEN
    IF ch2 > ch1 THEN
      IF ch1 < ch2 THEN
        Out.String("PASS"); Out.Ln;
        RETURN
      END
    END
  END;
  Out.String("FAIL"); Out.Ln
END Test;

BEGIN
    ch1 := 0AX;
    ch2 := 0FX;
    Test
END Relations8.
(*
    CHECK-NOT: FAIL
    CHECK: PASS
*)