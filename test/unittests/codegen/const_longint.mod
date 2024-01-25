(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  Valid 64bit integers not correctly parsed.
  Is there need to mark constant as LONGINT like C/C++?
  Maybe LONG(0)?
*)
MODULE ConstLongInt;

CONST
  longintmax = 9223372036854775807;
  longintmin = -9223372036854775808;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  printf("%lld\n", longintmax);
  printf("%lld\n", longintmin)
END Test;

BEGIN
    Test()
END ConstLongInt.
(*
    CHECK: 9223372036854775807
    CHECK: -9223372036854775808
*)