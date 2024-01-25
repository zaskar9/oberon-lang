(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  32bit hex constants not correctly parsed
*)
MODULE ConstHex;

CONST
  hexmax = 0FFFFFFFFH;
  hexdbg = 0DEADBEEFH;
  hexmin = 080000000H;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  printf("%X\n", hexmax);
  printf("%X\n", hexdbg);
  printf("%X\n", hexmin)
END Test;

BEGIN
    Test()
END ConstHex.
(*
    CHECK: FFFFFFFF
    CHECK: DEADBEEF
    CHECK: 80000000
*)