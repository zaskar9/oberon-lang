(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Arithmetic4;

IMPORT Out;

PROCEDURE Test;
VAR a, b, c : INTEGER;
BEGIN
  c := 10 DIV 3;
  Out.Int(c, 0);  Out.Ln;
  c := 10 MOD 3;
  Out.Int(c, 0);  Out.Ln;
  a := 10; b := 3;
  c := a DIV b;
  Out.Int(c, 0);  Out.Ln;
  c := a MOD b;
  Out.Int(c, 0);  Out.Ln
END Test;

BEGIN
    Test
END Arithmetic4.
(*
    CHECK: 3
    CHECK: 1
    CHECK: 3
    CHECK: 1
*)