(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE FailOnMissingSemicolon;

PROCEDURE Test;
BEGIN; END Test;

BEGIN
    Test;;
END FailOnMissingSemicolon.
(*
    CHECK: {{.*}}redundant semicolon{{.*}}
*)