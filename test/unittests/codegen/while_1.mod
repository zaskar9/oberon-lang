(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE While1;

IMPORT Out;

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
    Out.Int(x, 0); Out.Ln;
    Out.Int(y, 0); Out.Ln;
    Out.Int(z, 0); Out.Ln
END Test;

BEGIN
    Test
END While1.
(*
    CHECK: 3
    CHECK: 3
    CHECK: 3
*)