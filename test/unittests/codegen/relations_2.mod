(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Relations2;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  IF 2 # 1 THEN
    IF 2 >= 2 THEN
      IF 2 <= 3 THEN
        printf("PASS");
        RETURN
      END
    END
  END;
  printf("FAIL")
END Test;

BEGIN
    Test
END Relations2.
(*
    CHECK: PASS
*)