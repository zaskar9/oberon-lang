MODULE DisposeTest;
IMPORT Files, Out;

(*
TYPE
    Handle = POINTER TO RECORD END;  (* Declare FILE* data type from C <stdio.h> library. *)
    File* = POINTER TO RECORD
                name: ARRAY 32 OF CHAR;
                handle: Handle;
                registered, modified: BOOLEAN;
                next: File
            END;
*)
VAR f: Files.File;
    (* l: File; *)

  PROCEDURE Test;
  TYPE File = POINTER TO RECORD END;
  END Test;

BEGIN
    NEW(f);
    Out.String("File handle created."); Out.Ln;
    DISPOSE(f);
    Out.String("File handle disposed."); Out.Ln
END DisposeTest.
