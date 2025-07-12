(*
  RUN: echo 'X' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InCharTest;
IMPORT In, Out;

VAR ch: CHAR;

BEGIN
    ASSERT(~In.Done);
    In.Open();
    ASSERT(In.Done);
    In.Char(ch); Out.Char(ch); Out.Ln;
    ASSERT(In.Done);
END InCharTest.
(*
  CHECK: X
*)