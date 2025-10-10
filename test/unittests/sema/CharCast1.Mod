(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE CharCast1;
IMPORT Out;

  CONST LetterZ = "Z";

  PROCEDURE WriteLn(str: ARRAY OF CHAR);
  BEGIN
    Out.String(str); Out.Ln
  END WriteLn;

BEGIN
  WriteLn("X");
  WriteLn(41X);
  WriteLn(LetterZ);
  WriteLn("Hello!")
END CharCast1.
(*
  CHECK: X
  CHECK: A
  CHECK: Z
  CHECK: Hello!
*)