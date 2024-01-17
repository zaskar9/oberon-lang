(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE For4;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x, y : INTEGER;
BEGIN
    FOR x := 6 TO -1 BY -2 DO printf("%d\n", x) END
END Test;

BEGIN
    Test
END For4.
(*
    CHECK: 6
    CHECK: 4
    CHECK: 2
    CHECK: 0
*)