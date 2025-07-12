(*
  RUN: echo %s | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InNameTest;
IMPORT In, Out;

VAR name: ARRAY 256 OF CHAR;

BEGIN
    ASSERT(~In.Done);
    In.Open();
    ASSERT(In.Done);
    In.Name(name);
    ASSERT(In.Done);
    Out.String(name); Out.Ln
END InNameTest.
(*
  CHECK: {{.*}}InNameTest.Mod
*)