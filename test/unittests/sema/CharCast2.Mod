(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE CharCast1;
IMPORT Out;

  CONST LetterZ = "Z";

  VAR
    ch: CHAR;

  PROCEDURE WriteLn(VAR str: ARRAY OF CHAR);
  BEGIN
    Out.String(str); Out.Ln
  END WriteLn;

BEGIN
  WriteLn("X");
  WriteLn(41X);
  WriteLn(LetterZ);
  WriteLn(ch);
  WriteLn("Hello!")
END CharCast1.
(*
  CHECK: {{.*}}:18:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
  CHECK: {{.*}}:19:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
  CHECK: {{.*}}:20:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
  CHECK: {{.*}}:21:11:{{.*}}error:{{.*}}type mismatch: a non-constant character expression is not compatible with string.
  CHECK: {{.*}}:22:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
*)