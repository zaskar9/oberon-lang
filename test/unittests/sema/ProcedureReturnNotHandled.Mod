(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE ProcedureReturnNotHandled;

PROCEDURE Test(): INTEGER;
BEGIN
  RETURN 123
END Test;

BEGIN
    Test()
END ProcedureReturnNotHandled.
(*
    CHECK: {{.*}}12:5:{{.*}}warning{{.*}}discarded expression value.
*)