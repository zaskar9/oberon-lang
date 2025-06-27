(*
  RUN: echo '"Hello World!"' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InStringTest;
IMPORT In, Out;

VAR s: ARRAY 32 OF CHAR;

BEGIN
    ASSERT(~In.Done);
    In.Open();
    ASSERT(In.Done);
    In.String(s);
    ASSERT(In.Done);
    Out.String(s); Out.Ln
END InStringTest.
(*
  CHECK: Hello World!
*)