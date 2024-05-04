(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  This should fail or give a warning as it is not ROL, but ROR.
  OBNC gives a warning on negative value.
  Here we currently gives an error for negative literal values.
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