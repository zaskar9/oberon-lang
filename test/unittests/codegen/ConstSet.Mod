(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  SET type not supported yet
*)
MODULE ConstSet;
IMPORT Out;

CONST
  WordSize = 32;
  all = {0 .. WordSize-1};

PROCEDURE Test;
BEGIN
    Out.Hex(ORD(all)); Out.Ln
END Test;

BEGIN
    Test()
END ConstSet.
(*
    CHECK: FFFFFFFF
*)