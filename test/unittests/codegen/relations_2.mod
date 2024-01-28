(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Relations2;

IMPORT Out;

PROCEDURE Test;
BEGIN
  IF 2 # 1 THEN
    IF 2 >= 2 THEN
      IF 2 <= 3 THEN
        Out.String("PASS");
        RETURN
      END
    END
  END;
  Out.String("FAIL")
END Test;

BEGIN
    Test
END Relations2.
(*
    CHECK-NOT: FAIL
    CHECK: PASS
*)