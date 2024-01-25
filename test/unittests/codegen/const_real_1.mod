(*
  RUN: %oberon --run %s | filecheck %s
  Not sure what is happening here.
*)
MODULE ConstReal1;

CONST
  min = 1.1754939E-38;
  max = 3.4028235E38;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR
  val : LONGREAL;
  rval : REAL;
BEGIN
  rval := min;
  val := rval;
  printf("%.9g\n", val);
  rval := max;
  val := rval;
  printf("%.9g\n", val)
END Test;

BEGIN
    Test()
END ConstReal1.
(*
    CHECK: 1.17549393e-038
    CHECK: 3.4028235e038
*)