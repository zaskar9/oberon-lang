(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE For3;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x, y : INTEGER;
BEGIN
    y := 7;
    FOR x := 0 TO y BY 2 DO printf("%d\n", x) END
END Test;

BEGIN
    Test
END For3.
(*
    CHECK: 0
    CHECK: 2
    CHECK: 4
    CHECK: 6
*)