(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Arithmetic2;

IMPORT Out;

PROCEDURE Test;
VAR a, b : INTEGER;
BEGIN
  a := 2147483646;
  b := a + 1;
  Out.Int(b, 0);  Out.Ln;
  a := -2147483647;
  b := a - 1;
  Out.Int(b, 0);  Out.Ln
END Test;

BEGIN
    Test
END Arithmetic2.
(*
    CHECK: 214748364
    CHECK: -2147483648
*)