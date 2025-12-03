(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE TrapCopyOverflow1;
IMPORT Out;

VAR str: ARRAY 10 OF CHAR;

PROCEDURE assign(VAR dst: ARRAY OF CHAR; src: ARRAY OF CHAR);
BEGIN
    dst := src
END assign;

BEGIN
    assign(str, "Hello World!");
    Out.String(str); Out.Ln
END TrapCopyOverflow1.
(*
  CHECK: {{.*}}code 3 (array or string copy overflow)
*)