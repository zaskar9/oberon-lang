(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE UnterminatedString;

CONST s = "unterminated string literal;

END UnterminatedString.
(*
  CHECK: {{.*}}:6:11:{{.*}}error:{{.*}}string literal not terminated.
*)
