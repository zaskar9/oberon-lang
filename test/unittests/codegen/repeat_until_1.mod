(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE RepeatUntil1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x, y, z : INTEGER;
BEGIN
    x := 0;
    REPEAT
      y := 0;
      REPEAT
        z := 0;
        REPEAT
          z := z + 1
        UNTIL z > 2;
        y := y + 1
      UNTIL y > 2;
      x := x + 1
    UNTIL x > 2;
    printf("%d %d %d\n", x, y, z)
END Test;

BEGIN
    Test
END RepeatUntil1.
(*
    CHECK: 3 3 3
*)