(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE DuplicateImport;
IMPORT SYSTEM, SYSTEM;
END DuplicateImport.
(*
  CHECK: {{.*}}:5:16:{{.*}}error:{{.*}}duplicate import of module {{.*}}.
*)