(*
  RUN: %oberon --run %s | filecheck %s
  Note : Access to outer scope as here is i believe not alowed in Oberon-07.
*)
MODULE Procedure4;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test(a, b : INTEGER) : INTEGER;
    PROCEDURE Inner : INTEGER;
    BEGIN RETURN a + b
    END Inner;
BEGIN RETURN Inner()
END Test;

BEGIN
    printf("%d", Test(1, 2))
END Procedure4.
(*
    CHECK: 3
*)