(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE For2;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR x : INTEGER;
BEGIN
    FOR x := 0 TO 0 DO printf("%d\n", x) END;
    FOR x := 0 TO -1 DO printf("%d\n", x) END;
    printf("PASS")
END Test;

BEGIN
    Test
END For2.
(*
    CHECK: PASS
*)