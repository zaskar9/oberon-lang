(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE StringEscape;
IMPORT Out;

BEGIN
  Out.String("\\n"); Out.Ln
END StringEscape.
(*
  CHECK: \n
*)
