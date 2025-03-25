(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Illegal instruction
*)
MODULE Relations5;

IMPORT Out;

PROCEDURE Test;
BEGIN
  IF 2.5 = 2.5 THEN
    IF 2.5 > -1.5 THEN
      IF -1.5 < 2.5 THEN
        Out.String("PASS");
        RETURN
      END
    END
  END;
  Out.String("FAIL")
END Test;

BEGIN
    Test
END Relations5.
(*
    CHECK-NOT: FAIL
    CHECK: PASS
*)