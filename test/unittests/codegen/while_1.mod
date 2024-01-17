(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE While1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x, y, z : INTEGER;
BEGIN
    x := 0;
    WHILE x < 3 DO
        y := 0;
        WHILE y < 3 DO
            z := 0;
            WHILE z < 3 DO
                z := z + 1
            END;
            y := y + 1
        END;
        x := x + 1
    END;
    printf("%d %d %d\n", x, y, z)
END Test;

BEGIN
    Test
END While1.
(*
    CHECK: 3 3 3
*)