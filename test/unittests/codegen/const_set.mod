(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  SET type not supported yet
*)
MODULE ConstSet;

CONST
  WordSize = 32;
  all = {0 .. WordSize-1}

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
    printf("%X\n", ORD(all))
END Test;

BEGIN
    Test()
END ConstSet.
(*
    CHECK: FFFFFFFF
*)