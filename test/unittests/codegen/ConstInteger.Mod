(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Valid minimum 32bit integer not correctly parsed
*)
MODULE ConstInteger;

IMPORT Out;

CONST
  intmax = 2147483647;
  intzero = 0;
  intmin = -2147483648;

PROCEDURE Test;
BEGIN
  Out.Int(intmax, 0); Out.Ln;
  Out.Int(intzero, 0); Out.Ln;
  Out.Int(intmin, 0); Out.Ln
END Test;

BEGIN
    Test()
END ConstInteger.
(*
    CHECK: 2147483647
    CHECK: 0
    CHECK: -2147483648
*)