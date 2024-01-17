(*
  RUN: %oberon --run %s | filecheck %s
  UNSUPPORTED: *
  char constants not supported
*)
MODULE ConstChar;

CONST
  A = 0AX;
  B = 22X;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
BEGIN
    printf("%d %d\n", ORD(A), ORD(B))
END Test;

BEGIN
    Test()
END ConstChar.
(*
    CHECK: 10 32
*)