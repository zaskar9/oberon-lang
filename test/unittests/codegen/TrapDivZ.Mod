(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE TrapDivZ;

VAR i, j: INTEGER;

BEGIN
    i := 0;
    j := i DIV i
END TrapDivZ.
(*
  CHECK: {{.*}}code 6 (integer division by zero)
*)