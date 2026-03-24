(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE StringNullByte;
IMPORT Out;

VAR s: ARRAY 8 OF CHAR;

BEGIN
  s := "a\0b";
  Out.Int(ORD(s[1]), 0); Out.Ln
END StringNullByte.
(*
  CHECK: 0
*)
