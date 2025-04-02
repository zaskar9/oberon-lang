(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  This should fail or give a warning it is not left shift and gives garbage result and is undefined in C/C++.
  OBNC gives a warning on negative value, while oberon-lang reports an error for negative literal values.
*)
MODULE BuiltinAsr2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 16;
    Out.Int(ASR(i, -1), 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinAsr2.