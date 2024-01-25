(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  64bit hex constants not correctly parsed
  Is there need to mark constant as 64bit like C/C++?
  Maybe LONG(00H)?
*)
MODULE ConstLongHex;

CONST
  hexmax = 0FFFFFFFFFFFFFFFFH;
  hexdbg = 0DEADBEEFDEADBEEFH;
  hexmin = 08000000000000000H;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  printf("%llX\n", hexmax);
  printf("%llX\n", hexdbg);
  printf("%llX\n", hexmin)
END Test;

BEGIN
    Test()
END ConstLongHex.
(*
    CHECK: FFFFFFFFFFFFFFFF
    CHECK: DEADBEEFDEADBEEF
    CHECK: 8000000000000000
*)