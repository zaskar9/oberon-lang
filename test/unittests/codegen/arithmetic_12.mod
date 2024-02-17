(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
  Oberon-07 support mixing BYTE with INTEGER type.
  It seems BYTE type is not yet supported.
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