(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Valid 64bit integers not correctly parsed.
  Is there need to mark constant as LONGINT like C/C++?
  Maybe LONG(0)?
*)
MODULE ConstLongInt;

IMPORT Out;

CONST
  longintmax = 9223372036854775807;
  longintmin = -9223372036854775808;

PROCEDURE Test;
BEGIN
  Out.Int(longintmax, 0); Out.Ln;
  Out.Int(longintmin, 0); Out.Ln
END Test;

BEGIN
    Test()
END ConstLongInt.
(*
    CHECK: 9223372036854775807
    CHECK: -9223372036854775808
*)