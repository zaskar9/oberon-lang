(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
MODULE HelloworldFile;

IMPORT Files, Strings;

CONST OberonEOL = 1; UnixEOL = 2; WindowsEOL = 3;

VAR
  (* holds the eol marker type to use in WriteLn() *)
  eolType : INTEGER;
  (* Define a file handle *)
    f : Files.File;
  (* Define a file rider *)
    r : Files.Rider;

PROCEDURE WriteLn(VAR r : Files.Rider);
BEGIN
  IF eolType = WindowsEOL THEN
    (* A DOS/Windows style line ending, LFCR *)
    Files.Write(r, 13);
    Files.Write(r, 10);
  ELSIF eolType = UnixEOL THEN
     (* Linux/macOS style line ending, LF *)
     Files.Write(r, 10);
  ELSE
    (* Oberon, RISC OS style line ending, CR *)
    Files.Write(r, 13);
  END;
END WriteLn;

PROCEDURE WriteString(VAR r : Files.Rider; s : ARRAY OF CHAR);
  VAR i : INTEGER;
BEGIN
  i := 0;
  WHILE i < Strings.Length(s) DO
    Files.Write(r, ORD(s[i]));
    INC(i);
  END;
END WriteString;

BEGIN
  (* Set the desired eol type to use *)
  eolType := UnixEOL;
  (* Create our file, New returns a file handle *)
  f := Files.New("helloworld.txt"); ASSERT(f # NIL);
  (* Register our file with the file system *)
  Files.Register(f);
  (* Set the position of the rider to the beginning *)
  Files.Set(r, f, 0);
  (* Use the rider to write out "Hello World!" followed by a end of line *)
  WriteString(r, "Hello World!");
  WriteLn(r);
  (* Close our modified file *)
  Files.Close(f);
END HelloworldFile.
