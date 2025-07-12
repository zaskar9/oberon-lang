(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE FilesTest;
IMPORT Files, Out;

VAR f: Files.File;
    r: Files.Rider;
    res: INTEGER;
    len, pos: LONGINT;
    wShort, rShort: SHORTINT;
    wInt, rInt: INTEGER;
    wLong, rLong: LONGINT;
    wReal, rReal: REAL;
    wDouble, rDouble: LONGREAL;
    wSet, rSet: SET;
    x: BYTE;
    str: ARRAY 256 OF CHAR;
    t, d: LONGINT;

BEGIN
    f := Files.New("test.tmp");
    ASSERT(f # NIL);
    (* Out.String("File created."); Out.Ln; *)

    Files.Register(f);
    (* Out.String("File registered."); Out.Ln; *)
    DISPOSE(f);

    f := Files.Old("test.tmp");
    ASSERT(f # NIL);
    Files.GetName(f, str);
    ASSERT(str = "test.tmp");
    (* Out.String("File opened."); Out.Ln; *)

    Files.Close(f);
    (* Out.String("File closed."); Out.Ln; *)
    DISPOSE(f);

    res := 0;
    Files.Rename("test.tmp", "test.txt", res);
    ASSERT(res = 0);
    (* Out.String("File renamed."); Out.Ln; *)

    f := Files.Old("test.txt");
    ASSERT(f # NIL);
    (* Out.String("Renamed file opened."); Out.Ln; *)

    Files.GetName(f, str);
    ASSERT(str = "test.txt");
    (* Out.String("File name changed."); Out.Ln; *)

    len := Files.Length(f);
    ASSERT(len = 0);
    (* Out.String("File length: "); Out.Long(len, 0); Out.Char("."); Out.Ln; *)

    Files.Set(r, f, len);
    ASSERT(~r.eof);
    ASSERT(Files.Base(r) = f);
    ASSERT(Files.Pos(r) = len);
    (* Out.String("Rider set to end of file."); Out.Ln; *)

    Files.Read(r, x);
    ASSERT(r.res = 1);
    (* Out.String("Read failed as expected."); Out.Ln; *)

    x := 65;
    Files.Write(r, x);
    ASSERT(r.res = 0);
    (* Out.String("Write succeeded."); Out.Ln; *)
    x := 0;
    Files.Set(r, f, 0);
    Files.Read(r, x);
    ASSERT(r.res = 0);
    ASSERT(x = 65);
    (* Out.String("Read succeeded: "); Out.Char(CHR(x)); Out.Char("."); Out.Ln; *)

    len := Files.Length(f);
    ASSERT(len = 1);
    (* Out.String("File length: "); Out.Long(len, 0); Out.Char("."); Out.Ln; *)

    pos := Files.Pos(r);
    Files.WriteString(r, "Hello World!");
    Files.Set(r, f, pos);
    Files.ReadString(r, str);
    ASSERT(str = "Hello World!");
    (* Out.String("Read string succeeded: "); Out.String(str); Out.Char("."); Out.Ln; *)

    len := Files.Length(f);
    ASSERT(len = 14);
    (* Out.String("File length: "); Out.Long(len, 0); Out.Char("."); Out.Ln; *)

    pos := Files.Pos(r);
    wLong := MAX(LONGINT);
    Files.WriteNum(r, wLong);
    (* Out.String("Write number succeeded: "); Out.Long(wLong, 0); Out.Char("."); Out.Ln; *)
    Files.Set(r, f, pos);
    Files.ReadNum(r, rLong);
    (* Out.String("Read number succeeded: "); Out.Long(rLong, 0); Out.Char("."); Out.Ln; *)
    ASSERT(wLong = rLong);

    pos := Files.Pos(r);
    wLong := MIN(LONGINT);
    Files.WriteLInt(r, wLong);
    Files.Set(r, f, pos);
    Files.ReadLInt(r, rLong);
    ASSERT(wLong = rLong);

    pos := Files.Pos(r);
    wInt := MIN(INTEGER);
    Files.WriteInt(r, wInt);
    (* Out.String("Write integer succeeded: "); Out.Int(wInt, 0); Out.Char("."); Out.Ln; *)
    Files.Set(r, f, pos);
    Files.ReadInt(r, rInt);
    (* Out.String("Read integer succeeded: "); Out.Int(rInt, 0); Out.Char("."); Out.Ln; *)
    ASSERT(wInt = rInt);

    pos := Files.Pos(r);
    wShort := MIN(SHORTINT);
    Files.WriteSInt(r, wShort);
    Files.Set(r, f, pos);
    Files.ReadSInt(r, rShort);
    ASSERT(wShort = rShort);

    pos := Files.Pos(r);
    wReal := 3.40282347E+38;
    Files.WriteReal(r, wReal);
    Files.Set(r, f, pos);
    Files.ReadReal(r, rReal);
    ASSERT(wReal = rReal);

    pos := Files.Pos(r);
    wDouble := 1.7976931348623158E+308;
    Files.WriteLReal(r, wDouble);
    Files.Set(r, f, pos);
    Files.ReadLReal(r, rDouble);
    ASSERT(wDouble = rDouble);

    pos := Files.Pos(r);
    wSet := { 0, 3 .. 15, 31 };
    Files.WriteSet(r, wSet);
    Files.Set(r, f, pos);
    Files.ReadSet(r, rSet);
    ASSERT(wSet = rSet);

    Files.Purge(f);
    len := Files.Length(f);
    ASSERT(len = 0);

    Files.GetDate(f, t, d);
    Out.Long(d MOD 32, 0); Out.Char("-"); Out.Long(d DIV 32 MOD 16, 0); Out.Char("-"); Out.Long(d DIV 512, 0); Out.Char(" ");
    Out.Long(t DIV 4096, 0); Out.Char(":"); Out.Long(t DIV 64 MOD 64, 0); Out.Char(":"); Out.Long(t MOD 64, 0); Out.Ln;

    Files.Close(f);
    (* Out.String("Renamed file closed."); Out.Ln; *)
    DISPOSE(f);

    res := 0;
    Files.Delete("test.txt", res);
    ASSERT(res = 0);
    (* Out.String("Renamed file deleted."); Out.Ln; *)

    f := Files.Old("test.txt");
    ASSERT(f = NIL)
    (* Out.String("Deleted file does not exist."); Out.Ln *)

END FilesTest.