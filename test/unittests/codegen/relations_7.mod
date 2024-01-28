(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Relations7;

IMPORT Out;

PROCEDURE Test;
BEGIN
  IF "z" = "z" THEN
    IF "z" > "a" THEN
      IF "a" < "z" THEN
        Out.String("PASS");
        RETURN
      END
    END
  END;
  Out.String("FAIL")
END Test;

BEGIN
    Test
END Relations7.
(*
    CHECK-NOT: FAIL
    CHECK: PASS
*)