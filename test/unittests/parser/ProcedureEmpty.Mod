(*
  RUN: %oberon --run %s | filecheck %s
  It seems an empty procedure is not supported.
*)
MODULE ProcedureEmpty;

PROCEDURE Test;
BEGIN
END Test;

BEGIN
    Test()
END ProcedureEmpty.
(*
  CHECK: {{.*}}:9:1:{{.*}}warning:{{.*}}empty statement sequence.
*)