(*
  RUN: %oberon --run %s
*)
MODULE LongRealLiteral;

VAR d: LONGREAL;

BEGIN
    d := 1.0000000000000002220D+0;
    ASSERT(d # 1.0)
END LongRealLiteral.