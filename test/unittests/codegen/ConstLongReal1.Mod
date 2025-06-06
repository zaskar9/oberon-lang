(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  D exponent symbol for LONGREAL not supported. This is needed due to difference in rounding when parsing constants.
*)
MODULE ConstLongReal1;

IMPORT Out;

CONST
  min = -1.7976931348623158E308;
  max = -2.2250738585072014E-308;

PROCEDURE Test;
VAR
  rval : LONGREAL;
BEGIN
  rval := min;
  Out.LongReal(rval, 25); Out.Ln;
  rval := max;
  Out.LongReal(rval, 25); Out.Ln;
END Test;

BEGIN
    Test()
END ConstLongReal1.
(*
    CHECK: -1.797693134862316E+308
    CHECK: -2.225073858507201E-308
*)