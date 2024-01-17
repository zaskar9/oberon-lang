(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Const1;

CONST
  A = 1;
  B = 2 + A;
  S = "'";
  STR = "Oberon";
  SSTR = S + STR + S;
  F = 12.3;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test;
VAR r : LONGREAL;
BEGIN
    r := F;
    printf("%d %d %s %s %s %g\n", A, B, S, STR, SSTR, r)
END Test;

BEGIN
    Test()
END Const1.
(*
    CHECK: 1 3 ' Oberon 'Oberon' 12.3
*)