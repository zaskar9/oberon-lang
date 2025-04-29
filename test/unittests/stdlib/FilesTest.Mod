(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE FilesTest;
IMPORT Files, Out;

VAR f: Files.File;
    r: Files.Rider;
    res: INTEGER;
    len: LONGINT;
    x: BYTE;
    name: ARRAY 256 OF CHAR;

BEGIN
    f := Files.New("test.txt");
    ASSERT(f # NIL);
    Out.String("File created."); Out.Ln;

    Files.Register(f);
    Out.String("File registered."); Out.Ln;
    DISPOSE(f);

    f := Files.Old("test.txt");
    ASSERT(f # NIL);
    Out.String("File opened."); Out.Ln;

    res := 0;
    Files.Rename("test.txt", "test.bak", res);
    ASSERT(res = 0);
    Out.String("File renamed."); Out.Ln;

    Files.GetName(f, name);
    ASSERT(name = "test.bak");
    Out.String("File name changed."); Out.Ln;

    Files.Close(f);
    Out.String("File closed."); Out.Ln;
    DISPOSE(f);

    f := Files.Old("test.bak");
    ASSERT(f # NIL);
    Out.String("Renamed file opened."); Out.Ln;

    len := Files.Length(f);
    ASSERT(len >= 0);
    Out.String("File length: "); Out.Long(len, 0); Out.Char("."); Out.Ln;

    Files.Set(r, f, len);
    ASSERT(~r.eof);
    ASSERT(Files.Base(r) = f);
    ASSERT(Files.Pos(r) = len);
    Out.String("Rider set to end of file."); Out.Ln;

    Files.Read(r, x);
    Out.Int(x, 0); Out.Ln;
    ASSERT(r.eof);

    Files.Close(f);
    Out.String("Renamed file closed."); Out.Ln;
    DISPOSE(f);

    res := 0;
    Files.Delete("test.bak", res);
    ASSERT(res = 0);
    Out.String("Renamed file deleted."); Out.Ln;

    f := Files.Old("test.bak");
    ASSERT(f = NIL);
    Out.String("Deleted file does not exist."); Out.Ln

END FilesTest.