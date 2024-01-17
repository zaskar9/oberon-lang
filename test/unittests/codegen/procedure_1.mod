(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Procedure1;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test(a, b : INTEGER) : INTEGER;
BEGIN RETURN a + b
END Test;

BEGIN
    printf("%d", Test(1, 2))
END Procedure1.
(*
    CHECK: 3
*)