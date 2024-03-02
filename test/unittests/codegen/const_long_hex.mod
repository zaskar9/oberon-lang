(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  64bit hex constants not correctly parsed
  Is there need to mark constant as 64bit like C/C++?
  Maybe LONG(00H)?
*)
MODULE ConstLongHex;
IMPORT Out;

CONST
  hexmax = 0FFFFFFFFFFFFFFFFH;
  hexdbg = 0DEADBEEFDEADBEEFH;
  hexmin = 08000000000000000H;

PROCEDURE Test;
BEGIN
  Out.LongHex(hexmax); Out.Ln;
  Out.LongHex(hexdbg); Out.Ln;
  Out.LongHex(hexmin); Out.Ln
END Test;

BEGIN
    Test()
END ConstLongHex.
(*
    CHECK: FFFFFFFFFFFFFFFF
    CHECK: DEADBEEFDEADBEEF
    CHECK: 8000000000000000
*)