(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Oberon-07 supports mixing types BYTE and INTEGER without explicit casting.
*)
MODULE Arithmetic12;

IMPORT Out;

PROCEDURE Test;
VAR 
  i : BYTE;
  l, r : INTEGER;
BEGIN
  i := 7;
  l := 15;
  r := l + i;
  Out.Int(r, 0);  Out.Ln
END Test;

BEGIN
    Test
END Arithmetic12.
(*
    CHECK: 22
*)