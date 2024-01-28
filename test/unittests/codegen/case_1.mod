(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
  REQUIRES: revision
  CASE not supported
*)
MODULE Case1;

IMPORT Out;

PROCEDURE Test;
VAR i : INTEGER;
BEGIN
  i := 2;
  CASE i OF
    | 1 : Out.String("FAIL"); Out.Ln;
    | 2 : Out.String("PASS"); Out.Ln;
  END
END Test;

BEGIN
    Test
END Case1.
(*
    CHECK: PASS
*)