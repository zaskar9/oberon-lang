(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE CharCast1;
IMPORT Out;

  CONST LetterZ = "Z";

  PROCEDURE WriteLn(VAR str: ARRAY OF CHAR);
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
  CHECK: {{.*}}:15:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
  CHECK: {{.*}}:16:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
  CHECK: {{.*}}:17:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
  CHECK: {{.*}}:18:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a constant value by reference.
*)