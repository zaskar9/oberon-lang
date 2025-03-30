(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  32bit hex constants not correctly parsed
*)
MODULE ConstHex;
IMPORT Out;

CONST
  hexmax = 0FFFFFFFFH;
  hexdbg = 0DEADBEEFH;
  hexmin = 080000000H;


PROCEDURE Test;
BEGIN
  Out.Hex(hexmax); Out.Ln;
  Out.Hex(hexdbg); Out.Ln;
  Out.Hex(hexmin); Out.Ln
END Test;

BEGIN
    Test()
END ConstHex.
(*
    CHECK: FFFFFFFF
    CHECK: DEADBEEF
    CHECK: 80000000
*)