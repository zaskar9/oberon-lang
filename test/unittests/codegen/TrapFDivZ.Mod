(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE TrapFDivZ;

VAR i: INTEGER;
    r: REAL;

BEGIN
    i := 0;
    r := i / i
END TrapFDivZ.
(*
  CHECK: {{.*}}code 9 (floating point division by zero)
*)