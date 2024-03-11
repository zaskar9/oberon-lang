(*
  RUN: %oberon -fenable-extern -fenable-varargs --run %s | filecheck %s
  D exponent symbol for LONGREAL not supported.
  This is needed due to difference in rounding when parsing constants.
*)
MODULE ConstLongReal1;

CONST
  min = -1.7976931348623158E308;
  max = -2.2250738585072014E-308;

PROCEDURE printf(format: ARRAY OF CHAR; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR
  val : LONGREAL;
BEGIN
  val := min;
  printf("%.16G\n", val);
  val := max;
  printf("%.16G\n", val)
END Test;

BEGIN
    Test()
END ConstLongReal1.
(*
    CHECK: -1.797693134862316E+308
    CHECK: -2.225073858507201E-308
*)