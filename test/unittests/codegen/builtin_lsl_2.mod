(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  
  This should probably fail as it is not left shift and gives garbage result.
  Atleast constant arguments could be checked for overflow and negative shift.
  At runtime with variable argument it could be undefined.
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