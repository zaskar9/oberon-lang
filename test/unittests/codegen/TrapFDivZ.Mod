(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE TrapFDivZ;

VAR i: INTEGER;
    r: REAL;

BEGIN
    i := 0;
    r := i / i
END TrapFDivZ.