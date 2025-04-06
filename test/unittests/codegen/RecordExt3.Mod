(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE RecordExt3;
IMPORT Out, Texts;

TYPE
    MyWriterDesc = RECORD (Texts.Writer) flag: INTEGER END;
    MyWriter = POINTER TO MyWriterDesc;

VAR wtr: MyWriter;
    w: Texts.Writer;

PROCEDURE TestExt(VAR w: Texts.Writer);
BEGIN
    IF w IS MyWriterDesc THEN
        Out.String("MyWriterDesc")
    ELSIF w IS Texts.Writer THEN
        Out.String("Texts.Writer")
    ELSE
        Out.String("Error")
    END
END TestExt;

BEGIN
    NEW(wtr);
    ASSERT(wtr IS MyWriter);
    TestExt(wtr^); Out.Ln;
    TestExt(w); Out.Ln;
    DISPOSE(wtr)
END RecordExt3.
(*
  CHECK: MyWriterDesc
  CHECK: Texts.Writer
*)