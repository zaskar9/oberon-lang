(*
  RUN: echo '-2147483646 0FFH' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InIntegerTest;
IMPORT In, Out;

VAR i: INTEGER;

BEGIN
    ASSERT(~In.Done);
    In.Open();
    ASSERT(In.Done);
    In.Int(i);
    ASSERT(In.Done);
    Out.Int(i, 0); Out.Ln;
    In.Int(i);
    ASSERT(In.Done);
    Out.Int(i, 0); Out.Ln
END InIntegerTest.
(*
  CHECK: -2147483646
  CHECK: 255
*)