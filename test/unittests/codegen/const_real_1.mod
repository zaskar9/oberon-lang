(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
  Segmentation fault. Not sure what is happening here.
*)
MODULE ConstReal1;

IMPORT Out;

CONST
  min = 1.1754939E-38;
  max = 3.4028235E38;

PROCEDURE Test;
VAR
  rval : REAL;
BEGIN
  rval := min;
  Out.Real(rval); Out.Ln;
  rval := max;
  Out.Real(rval); Out.Ln
END Test;

BEGIN
    Test()
END ConstReal1.
(*
    CHECK: 1.17549393e-038
    CHECK: 3.4028235e038
*)