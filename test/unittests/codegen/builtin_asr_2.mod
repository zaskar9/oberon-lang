(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  
  This should probably fail as it is not arithmetic shift right and gives garbage result.
  Atleast constant arguments could be checked for overflow and negative shift.
  At runtime with variable argument it could be undefined.
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