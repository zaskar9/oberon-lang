(*
  RUN: echo 'Hello Niklaus!\nHoi Moira!' | %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InLineTest;
IMPORT In, Out;

VAR line: ARRAY 32 OF CHAR;

BEGIN
    In.Open;
    ASSERT(In.Done);
    In.Line(line);
    WHILE In.Done DO
        Out.String(line); Out.Ln;
        In.Line(line)
    END
END InLineTest.
(*
  CHECK: Hello Niklaus!
  CHECK: Hoi Moira!
*)