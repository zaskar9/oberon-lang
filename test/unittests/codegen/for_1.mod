(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE For1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
    FOR x := 0 TO 3 DO printf("%d\n", x) END
END Test;

BEGIN
    Test
END For1.
(*
    CHECK: 0
    CHECK: 1
    CHECK: 2
    CHECK: 3
*)