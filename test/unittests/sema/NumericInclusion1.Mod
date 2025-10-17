(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE NumericInclusion1;
IMPORT Out;

VAR
  s: SHORTINT;
  i: INTEGER;
  l: LONGINT;
  f: REAL;
  d: LONGREAL;

BEGIN
  s := 42;
  i := s;
  l := s; l := i;
  Out.Long(l, 0); Out.Ln;
  f := s; f := i; f := l;
  d := s; d := i; d := l; d := f;
  Out.LongReal(d, 0); Out.Ln
END NumericInclusion1.
(*
  CHECK: 42
  CHECK:  4.2E+001
*)
