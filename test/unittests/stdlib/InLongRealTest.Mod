(*
  RUN: echo 'd -2.2250738585072014E-308' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InLongRealTest;
IMPORT In, Out;

VAR ch: CHAR;
    d: LONGREAL;

BEGIN
    ASSERT(~In.Done);
    In.Open();
    ASSERT(In.Done);
    In.Char(ch);
    ASSERT(In.Done);
    IF ch = "d" THEN
        In.LongReal(d);
        ASSERT(In.Done);
        Out.LongReal(d, 15); Out.Ln
    END
END InLongRealTest.
(*
  CHECK: -2.2250739E-308
*)