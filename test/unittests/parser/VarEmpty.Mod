(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE VarEmpty;

VAR

END VarEmpty.
(*
  CHECK: {{.*}}:6:1:{{.*}}error:{{.*}}empty VAR declaration
*)