(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  According to the Oakwood Guidelines, Section 2.8, redefining predefined procedures should be possible.
*)
MODULE RedefineProcedure;
IMPORT Out;

PROCEDURE ABS;
BEGIN
    Out.String("OK"); Out.Ln
END ABS;

BEGIN
    ABS
END RedefineProcedure.
(*
  CHECK: OK
*)