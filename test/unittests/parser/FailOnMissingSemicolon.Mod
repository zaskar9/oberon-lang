(*
  RUN: not %oberon --run %s | filecheck %s
*)
MODULE FailOnMissingSemicolon;

PROCEDURE Test;
END Test;

BEGIN
    Test
    Test
END FailOnMissingSemicolon.
(*
    CHECK: {{.*}}:10:9:{{.*}}error:{{.*}}semicolon missing.
*)