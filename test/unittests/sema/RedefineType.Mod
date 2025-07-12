(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  According to the Oakwood Guidelines, Section 2.8, redefining predefined types should be possible.
*)
MODULE RedefineType;
IMPORT Out;

TYPE INTEGER = LONGINT;

BEGIN
    Out.Long(SIZE(INTEGER), 0); Out.Ln
END RedefineType.
(*
  CHECK: 8
*)