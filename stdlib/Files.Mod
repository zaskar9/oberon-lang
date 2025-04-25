(*NW 11.1.86 / 28.8.92 / pjm 17.04.97 / MG 24.04.2025 *)
MODULE Files;

TYPE
    File* = POINTER TO RECORD
                name: ARRAY 32 OF CHAR;
                id, t, d: LONGINT;
                onDisk, modified: BOOLEAN;
                next: File
            END;
    Rider* = RECORD                  (** Riders are the access mechanisms for files. *)
			    eof*: BOOLEAN;	     (** Rider has reached the end of the file. *)
				res*, pos: LONGINT;	 (** Rider operation result code. *)
				file: File
			END;

	Bytes4 = ARRAY 4 OF BYTE;
	Bytes8 = ARRAY 8 OF BYTE;

VAR
    root: File;

    (* Import `tmpfile()` function from C <stdio.h> library. *)
    PROCEDURE tmpfile(): LONGINT; EXTERN;

    (** Creates a new file with the specified name. *)
    PROCEDURE New*(name: ARRAY OF CHAR): File;
    VAR f: File; id: LONGINT;
    BEGIN
        id := tmpfile();
        NEW(f);
        f.name := name;
        f.id := id;
        RETURN f
    END New;

END Files.