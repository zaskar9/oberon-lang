(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE For2;

IMPORT Out;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
    FOR x := 0 TO 0 DO Out.Int(x, 0); Out.Ln END;
    FOR x := 0 TO -1 DO Out.Int(x, 0); Out.Ln END;
    Out.String("PASS"); Out.Ln
END Test;

BEGIN
    Test
END For2.
(*
    CHECK: PASS
*)