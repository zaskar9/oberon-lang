(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE ConstEmpty;

CONST

END ConstEmpty.
(*
  CHECK: {{.*}}:6:1:{{.*}}error:{{.*}}empty CONST declaration
*)