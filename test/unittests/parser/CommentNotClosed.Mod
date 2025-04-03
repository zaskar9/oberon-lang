(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE CommentNotClosed;

(*(**)

END CommentNotClosed.
(*
  CHECK: {{.*}}:6:1:{{.*}}error:{{.*}}comment not closed.
*)