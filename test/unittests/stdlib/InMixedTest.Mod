(*
  RUN: echo '123*1.5 "abc" Mod.Proc' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InMixedTest;
IMPORT In, Out;

VAR i: INTEGER; c: CHAR; r: REAL; s, n: ARRAY 32 OF CHAR;

BEGIN
    In.Open;
    ASSERT(In.Done);
    In.Int(i);
    ASSERT(In.Done);
    In.Char(c);
    ASSERT(In.Done);
    In.Real(r);
    ASSERT(In.Done);
    In.String(s);
    ASSERT(In.Done);
    In.Name(n);
    ASSERT(In.Done);
    Out.String("i = "); Out.Int(i, 0); Out.Ln;
    Out.String("c = "); Out.Char(c); Out.Ln;
    Out.String("r = "); Out.Real(r, 0); Out.Ln;
    Out.String("s = "); Out.String(s); Out.Ln;
    Out.String("n = "); Out.String(n); Out.Ln
END InMixedTest.
(*
  CHECK: i = 123
  CHECK: c = *
  CHECK: r =  1.50E+00
  CHECK: s = abc
  CHECK: n = Mod.Proc
*)