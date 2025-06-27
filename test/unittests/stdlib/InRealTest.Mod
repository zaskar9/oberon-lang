(*
  RUN: echo 'f 0.31415926e+1' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InRealTest;
IMPORT In, Out;

VAR ch: CHAR;
    f: REAL;

BEGIN
    ASSERT(~In.Done);
    In.Open();
    ASSERT(In.Done);
    In.Char(ch);
    ASSERT(In.Done);
    IF ch = "f" THEN
        In.Real(f);
        ASSERT(In.Done);
        Out.Real(f, 15); Out.Ln
    END
END InRealTest.
(*
  CHECK:  3.14159264E+00
*)