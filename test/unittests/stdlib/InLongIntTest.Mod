(*
  RUN: echo '-9223372036854775808 07FFFFFFFFFFFFFFFH' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InLongIntTest;
IMPORT In, Out;

VAR l: LONGINT;

BEGIN
    ASSERT(~In.Done);
    In.Open();
    ASSERT(In.Done);
    In.LongInt(l);
    ASSERT(In.Done);
    Out.Long(l, 0); Out.Ln;
    In.LongInt(l);
    ASSERT(In.Done);
    Out.Long(l, 0); Out.Ln
END InLongIntTest.
(*
  CHECK: -9223372036854775808
  CHECK: 9223372036854775807
*)