MODULE DisposeTest;
IMPORT Files, Out;

TYPE
    Handle = POINTER TO RECORD END;
    File* = POINTER TO RECORD
                name: ARRAY 32 OF CHAR;
                handle: Handle;
                flags: RECORD registered, modified, readonly: BOOLEAN END;
                next: File
            END;
    Rider* = POINTER TO Files.Rider;

VAR f: Files.File;
    l: File;

  PROCEDURE Test;
  TYPE File = POINTER TO RECORD END;
  END Test;

BEGIN
    NEW(f); NEW(l);
    Out.String("File handle created."); Out.Ln;
    DISPOSE(f); DISPOSE(l);
    Out.String("File handle disposed."); Out.Ln
END DisposeTest.
