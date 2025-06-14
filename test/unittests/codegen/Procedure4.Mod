(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Procedure4;
IMPORT Out;

PROCEDURE Test(a, b : INTEGER) : INTEGER;
    PROCEDURE Inner(): INTEGER;
    BEGIN
        RETURN a + b
    END Inner;
BEGIN RETURN Inner()
END Test;

BEGIN
    Out.Int(Test(1, 2), 0); Out.Ln
END Procedure4.
(*
    CHECK: 3
*)