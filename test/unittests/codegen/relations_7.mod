(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Relations7;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
  IF "z" = "z" THEN
    IF "z" > "a" THEN
      IF "a" < "z" THEN
        printf("PASS");
        RETURN
      END
    END
  END;
  printf("FAIL")
END Test;

BEGIN
    Test
END Relations7.
(*
    CHECK: PASS
*)