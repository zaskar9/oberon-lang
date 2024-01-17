(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  CASE not supported
*)
MODULE Case1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR i : INTEGER;
BEGIN
  i := 2;
  CASE i OF
    | 1 : printf("FAIL");
    | 2 : printf("PASS")
  END
END Test;

BEGIN
    Test
END Case1.
(*
    CHECK: PASS
*)