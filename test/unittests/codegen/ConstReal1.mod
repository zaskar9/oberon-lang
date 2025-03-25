(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Segmentation fault. Not sure what is happening here.
*)
MODULE ConstReal1;

IMPORT Out;

CONST
  min = 1.175494351E-38;
  max = 3.402823466E+38;

PROCEDURE Test;
VAR
  rval : REAL;
BEGIN
  rval := min;
  Out.Real(rval, 16); Out.Ln;
  rval := max;
  Out.Real(rval, 16); Out.Ln
END Test;

BEGIN
    Test()
END ConstReal1.
(*
    CHECK: 1.17549432E-38
    CHECK: 3.40282368E+38
*)