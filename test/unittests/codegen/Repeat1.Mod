(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Repeat1;

IMPORT Out;

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
    Out.Int(x, 0); Out.Ln;
    Out.Int(y, 0); Out.Ln;
    Out.Int(z, 0); Out.Ln
END Test;

BEGIN
    Test
END Repeat1.
(*
    CHECK: 3
    CHECK: 3
    CHECK: 3
*)