(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE FilesTest;
IMPORT Files, Out;

VAR f: Files.File;

BEGIN
    f := Files.New("test.txt");
    ASSERT(f # NIL);
    DISPOSE(f)
END FilesTest.