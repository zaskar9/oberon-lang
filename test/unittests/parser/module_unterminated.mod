(*
  RUN: %oberon --run %s
  XFAIL: *
  REQUIRES: revision
  Goes into a infinite loop
*)
MODULE ModuleUnterminated;
END ModuleUnterminated