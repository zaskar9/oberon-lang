(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Procedure3;

VAR
  a, b, c : INTEGER;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test(a, b : INTEGER; VAR c : INTEGER);
BEGIN c := a + b
END Test;

BEGIN
    a := 1; b := 2;
    Test(a, b, c);
    printf("%d", c)
END Procedure3.
(*
    CHECK: 3
*)