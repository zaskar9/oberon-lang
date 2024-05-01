(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  Should this compile?
*)
MODULE BuiltinLong2;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := LONG(123);
    Out.Int(i, 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinLong2.