(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Strings1;
IMPORT Out;

CONST hello = "Hello World!";

VAR str: ARRAY 30 OF CHAR;

PROCEDURE PrintAnswer(str: ARRAY OF CHAR);
BEGIN
    IF (str = "yes") OR (str = "y") THEN
        Out.String("Yippee!")
    ELSIF ("no" = str) OR (str = "n") THEN
        Out.String("Sorry!")
    END;
    Out.Ln
END PrintAnswer;

BEGIN
    str := "Hello World!";
    IF str # "" THEN
        PrintAnswer("yes");
        PrintAnswer("no");
        PrintAnswer("y");
        PrintAnswer("n")
    END
END Strings1.
(*
  CHECK: Yippee!
  CHECK: Sorry!
  CHECK: Yippee!
  CHECK: Sorry!
*)