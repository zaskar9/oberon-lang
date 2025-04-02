(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE WarnOnRedundantSemicolon;

PROCEDURE Test;
BEGIN;
END Test;

BEGIN
    Test;;
END WarnOnRedundantSemicolon.
(*
  CHECK: {{.*}}:7:6:{{.*}}warning:{{.*}}redundant semicolon.
  CHECK: {{.*}}:11:9:{{.*}}warning:{{.*}}redundant semicolon.
  CHECK: {{.*}}:11:10:{{.*}}warning:{{.*}}redundant semicolon.
*)