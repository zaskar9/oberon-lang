(*
  RUN: %oberon -fenable-extern -fenable-varargs --run %s | filecheck %s
  D exponent symbol for LONGREAL not supported.
  This is needed due to difference in rounding when parsing constants.
*)
MODULE ConstLongReal1;

CONST
  min = -1.7976931348623158E308;
  max = -2.2250738585072014E-308;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR
  val : LONGREAL;
BEGIN
  val := min;
  printf("%.16g\n", val);
  val := max;
  printf("%.16g\n", val)
END Test;

BEGIN
    Test()
END ConstLongReal1.
(*
    CHECK: -1.797693134862316e+308
    CHECK: -2.225073858507201e-308
*)