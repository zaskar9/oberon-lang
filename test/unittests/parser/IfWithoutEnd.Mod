(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
  Error message could be improved to complain about missing END of IF statement.
*)
MODULE IfWithoutEnd;

BEGIN
  IF 1 > 2 THEN HALT(1);
END IfWithoutEnd.
(*
  CHECK: {{.*}}:12:3:{{.*}}error:{{.*}}premature end of file.
*)