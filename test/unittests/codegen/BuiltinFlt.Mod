(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinFlt;

IMPORT Out;

PROCEDURE Test;
BEGIN
  Out.LongReal(FLT(0), 17); Out.Ln;
  Out.LongReal(FLT(1), 17); Out.Ln;
  Out.LongReal(FLT(9007199254740992), 17); Out.Ln (* Biggest exact integer 2^52 *)
END Test;

BEGIN
    Test
END BuiltinFlt.
(*
  CHECK: 0
  CHECK: 1.000000000E+000
  CHECK: 9.007199255E+015
  CHECK-EMPTY
*)
