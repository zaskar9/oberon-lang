(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Relations3;

IMPORT Out;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := 1; b := 2;
  IF b # a THEN
    IF b > a THEN
      IF a < b THEN
        Out.String("PASS");
        RETURN
      END
    END
  END;
  Out.String("FAIL")
END Test;

BEGIN
    Test
END Relations3.
(*
    CHECK-NOT: FAIL
    CHECK: PASS
*)