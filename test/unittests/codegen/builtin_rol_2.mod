(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  
  This should probably fail as it is not ROL, but ROR.
  Atleast constant arguments could be checked for overflow and negative shift.
  Note that this procedure is not found in Oberon-2 or Oberon-07 reports.
*)
MODULE BuiltinRol2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 0F000000FH;
    Out.Hex(ROL(i, -4)); Out.Ln
END Test;

BEGIN
    Test
END BuiltinRol2.