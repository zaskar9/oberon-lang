(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Fails : Basic Block in function 'Procedure5__Inner' does not have terminator!
*)
MODULE Procedure5;

IMPORT Out;

VAR
  a, b, c : INTEGER;

PROCEDURE Test();

    PROCEDURE Inner();
    BEGIN
        c := a + b
    END Inner;

BEGIN
    Inner()
END Test;

BEGIN
    a := 1; b := 2;
    Test;
    Out.Int(c, 0); Out.Ln
END Procedure5.
(*
    CHECK: 3
*)