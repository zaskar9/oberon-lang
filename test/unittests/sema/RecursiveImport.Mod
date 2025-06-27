(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE RecursiveImport;
IMPORT RecursiveImport;
END RecursiveImport.
(*
  CHECK: {{.*}}:5:8:{{.*}}error:{{.*}}module {{.*}} must not import itself.
*)