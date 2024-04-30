(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  
  This should probably fail as it is not ROR, but ROL.
  Atleast constant arguments could be checked for overflow and negative shift.
*)
MODULE BuiltinRol2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 0F000000FH;
    Out.Hex(ROR(i, -4)); Out.Ln
END Test;

BEGIN
    Test
END BuiltinRol2.