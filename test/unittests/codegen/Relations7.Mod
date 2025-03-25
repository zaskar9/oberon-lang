(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Relations7;

IMPORT Out;

PROCEDURE Test;
BEGIN
  IF "z" = "z" THEN
    IF "z" > "a" THEN
      IF "a" < "z" THEN
        Out.String("PASS"); Out.Ln;
        RETURN
      END
    END
  END;
  Out.String("FAIL"); Out.Ln
END Test;

BEGIN
    Test
END Relations7.
(*
    CHECK-NOT: FAIL
    CHECK: PASS
*)