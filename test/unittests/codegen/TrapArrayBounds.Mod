(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE TrapArrayBounds;

VAR a: ARRAY 3 OF INTEGER;
    i: INTEGER;

BEGIN
    FOR i := 0 TO 5 DO
        a[i] := i
    END
END TrapArrayBounds.
(*
  CHECK: {{.*}}code 1 (array index out of range)
*)