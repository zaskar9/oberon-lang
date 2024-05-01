(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  This should probably fail as it per definition is an overflow
  Maybe also variables should be checked for overflow?
*)
MODULE BuiltinShort2;

IMPORT Out;

PROCEDURE Test;
VAR
    s : INTEGER;
BEGIN
    s := SHORT(123456789);
    Out.Int(s, 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinShort2.