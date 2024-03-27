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
    CHECK: {{.*}}discarded expression value{{.*}}
*)