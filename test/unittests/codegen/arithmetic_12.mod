(*
  RUN: %oberon --run %s | filecheck %s
  Oberon-07 support BYTE with INTEGER type.
  It seems BYTE type is not yet supported.
*)
MODULE Arithmetic12;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR 
  i : BYTE;
  l, r : INTEGER;
BEGIN
  i := 7;
  l := 15;
  r := l + i;
  printf("%d", r)
END Test;

BEGIN
    Test
END Arithmetic12.
(*
    CHECK: 22
*)