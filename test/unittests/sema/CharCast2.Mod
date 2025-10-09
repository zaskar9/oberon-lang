(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE CharCast1;
IMPORT Out;

  PROCEDURE WriteLn(VAR str: ARRAY OF CHAR);
  BEGIN
    Out.String(str); Out.Ln
  END WriteLn;

BEGIN
  WriteLn("X");
  WriteLn(41X);
  WriteLn("Hello!")
END CharCast1.
(*
  CHECK: {{.*}}:13:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass literal by reference.
  CHECK: {{.*}}:14:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass literal by reference.
  CHECK: {{.*}}:15:11:{{.*}}error:{{.*}}illegal actual parameter: cannot pass literal by reference.
*)