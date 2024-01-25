(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  Valid minimum 32bit integer not correctly parsed
*)
MODULE ConstInteger;

CONST
  intmax = 2147483647;
  intzero = 0;
  intmin = -2147483648;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  printf("%d\n", intmax);
  printf("%d\n", intzero);
  printf("%d\n", intmin)
END Test;

BEGIN
    Test()
END ConstInteger.
(*
    CHECK: 2147483647
    CHECK: 0
    CHECK: -2147483648
*)