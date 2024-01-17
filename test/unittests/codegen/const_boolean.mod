(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  boolean constants not supported
*)
MODULE ConstBoolean;

CONST
  A = FALSE;
  B = TRUE;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
    printf("%d %d\n", ORD(A), ORD(B))
END Test;

BEGIN
    Test()
END ConstBoolean.
(*
    CHECK: 0 1
*)