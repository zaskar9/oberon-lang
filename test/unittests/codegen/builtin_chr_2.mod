(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
  Should maybe check for valid range for constant arguments
*)
MODULE BuiltinChr2;

IMPORT Out;

PROCEDURE Test;
BEGIN
    Out.Char(CHR(128)); Out.Ln; (* Overflow -128 to 127 *)
END Test;

BEGIN
    Test
END BuiltinChr2.