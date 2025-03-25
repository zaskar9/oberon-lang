(*
  RUN: %oberon --run %s
  XFAIL: *
*)
MODULE TrapIntOverflow;

VAR i: INTEGER;

BEGIN
    i := MAX(INTEGER);
    INC(i)
END TrapIntOverflow.