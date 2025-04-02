(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE ModuleUnterminated;
(*
  CHECK: {{.*}}:9:1:{{.*}}error:{{.*}}. expected, found <eof>.
*)
END ModuleUnterminated
