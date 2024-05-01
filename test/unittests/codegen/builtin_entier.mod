(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  ENTIER and FLOOR is similar.
*)
MODULE BuiltinEntier;

IMPORT Out;

PROCEDURE Test;
BEGIN
  Out.Long(ENTIER(1.5), 0); Out.Ln;
  Out.Long(ENTIER(-1.5), 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinEntier.
(*
  CHECK: 1
  CHECK: -2
  CHECK-EMPTY
*)