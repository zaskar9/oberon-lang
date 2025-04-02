(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE TypeEmpty;

TYPE

END TypeEmpty.
(*
  CHECK: {{.*}}:6:1:{{.*}}error:{{.*}}empty TYPE declaration
*)