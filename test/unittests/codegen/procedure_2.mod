(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Procedure2;

VAR c : INTEGER;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test(a, b : INTEGER; VAR c : INTEGER);
BEGIN c := a + b
END Test;

BEGIN
    Test(1, 2, c);
    printf("%d", c)
END Procedure2.
(*
    CHECK: 3
*)