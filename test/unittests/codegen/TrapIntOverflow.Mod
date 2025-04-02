(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE TrapIntOverflow;

VAR i: INTEGER;

BEGIN
    i := MAX(INTEGER);
    INC(i)
END TrapIntOverflow.
(*
  CHECK: {{.*}}code 8 (integer overflow)
*)