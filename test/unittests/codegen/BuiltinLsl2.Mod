(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  This should fail or give a warning it is not left shift and gives garbage result and is undefined in C/C++.
  OBNC gives a warning on negative value,  while oberon-lang reports an error for negative literal values.
  Project Oberon uses negative value one place in Texts.Mod.
*)
MODULE BuiltinLsl2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
    l : LONGINT;
BEGIN
    i := 0FFH;
    Out.Hex(LSL(i, -4)); Out.Ln
END Test;

BEGIN
    Test
END BuiltinLsl2.