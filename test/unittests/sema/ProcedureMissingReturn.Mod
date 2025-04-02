(*
  RUN: not %oberon --run %s | filecheck %s
*)
MODULE ProcedureMissingReturn;

VAR
  ret : INTEGER;

PROCEDURE Test(): INTEGER;
BEGIN
  IF FALSE THEN RETURN 123 END
END Test;

BEGIN
   ret := Test()
END ProcedureMissingReturn.
(*
    CHECK: {{.*}}9:1:{{.*}}error:{{.*}}not all control flow paths of the procedure return a result.
*)