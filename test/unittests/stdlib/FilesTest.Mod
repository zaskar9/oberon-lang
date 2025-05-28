(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE FilesTest;
IMPORT Files, Out;

VAR f: Files.File;
    r: Files.Rider;
    res: INTEGER;
    len, pos, num: LONGINT;
    x: BYTE;
    str: ARRAY 256 OF CHAR;

BEGIN
    f := Files.New("test.bin");
    ASSERT(f # NIL);
    Out.String("File created."); Out.Ln;

    Files.Register(f);
    Out.String("File registered."); Out.Ln;
    DISPOSE(f);

    f := Files.Old("test.bin");
    ASSERT(f # NIL);
    Out.String("File opened."); Out.Ln;

    res := 0;
    Files.Rename("test.bin", "test.txt", res);
    ASSERT(res = 0);
    Out.String("File renamed."); Out.Ln;

    Files.GetName(f, str);
    ASSERT(str = "test.txt");
    Out.String("File name changed."); Out.Ln;

    Files.Close(f);
    Out.String("File closed."); Out.Ln;
    DISPOSE(f);

    f := Files.Old("test.txt");
    ASSERT(f # NIL);
    Out.String("Renamed file opened."); Out.Ln;

    len := Files.Length(f);
    ASSERT(len = 0);
    Out.String("File length: "); Out.Long(len, 0); Out.Char("."); Out.Ln;

    Files.Set(r, f, len);
    ASSERT(~r.eof);
    ASSERT(Files.Base(r) = f);
    ASSERT(Files.Pos(r) = len);
    Out.String("Rider set to end of file."); Out.Ln;

    Files.Read(r, x);
    ASSERT(r.res = 1);
    Out.String("Read failed as expected."); Out.Ln;

    x := 65;
    Files.Write(r, x);
    ASSERT(r.res = 0);
    Out.String("Write succeeded."); Out.Ln;

    Files.Set(r, f, 0);
    Files.Read(r, x);
    ASSERT(r.res = 0);
    Out.String("Read succeeded: "); Out.Char(CHR(x)); Out.Char("."); Out.Ln;

    len := Files.Length(f);
    ASSERT(len = 1);
    Out.String("File length: "); Out.Long(len, 0); Out.Char("."); Out.Ln;

    pos := Files.Pos(r);
    Files.WriteString(r, "Hello World!");
    Files.Set(r, f, pos);
    Files.ReadString(r, str);
    ASSERT(str = "Hello World!");
    Out.String("Read string succeeded: "); Out.String(str); Out.Char("."); Out.Ln;

    len := Files.Length(f);
    ASSERT(len = 14);
    Out.String("File length: "); Out.Long(len, 0); Out.Char("."); Out.Ln;

    pos := Files.Pos(r);
    num := MAX(LONGINT);
    Files.WriteNum(r, num);

    Out.String("Write number succeeded: "); Out.Long(num, 0); Out.Char("."); Out.Ln;
    Files.Set(r, f, pos);
    Files.ReadNum(r, num);
    Out.String("Read number succeeded: "); Out.Long(num, 0); Out.Char("."); Out.Ln;

    Files.Close(f);
    Out.String("Renamed file closed."); Out.Ln;
    DISPOSE(f);

    res := 0;
    Files.Delete("test.txt", res);
    ASSERT(res = 0);
    Out.String("Renamed file deleted."); Out.Ln;

    f := Files.Old("test.txt");
    ASSERT(f = NIL);
    Out.String("Deleted file does not exist."); Out.Ln

END FilesTest.