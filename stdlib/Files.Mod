(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** The Files module implements the Oberon file system. *)
MODULE Files; (*NW 11.1.86 / 28.8.92 / pjm 17.04.97 / MG 24.04.2025 *)

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


    (* Declare `FILE *tmpfile(void)` function from C <stdio.h> library. *)
    PROCEDURE tmpfile(): LONGINT; EXTERN;

    (* Declare `int32_t olang_files_file_exist(const char*, const void * ) from Oberon runtime library. *)
    PROCEDURE olang_files_file_exist(name: ARRAY OF CHAR): INTEGER; EXTERN;

    (* Declare `FILE *olang_files_file_exist(const char*, const void * ) from Oberon runtime library. *)
    PROCEDURE olang_files_file_open(name: ARRAY OF CHAR): LONGINT; EXTERN;


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

    (** Open an existing file. The same file descriptor is returned if a file is opened multiple times. *)
    PROCEDURE Old*(name: ARRAY OF CHAR): File;
    VAR f: File; id: LONGINT;
    BEGIN
        f := root;
        WHILE f # NIL DO
          IF f.name = name THEN RETURN f END;
          f := f.next
        END;
        IF olang_files_file_exist(name) = 0 THEN RETURN NIL END;
        id := olang_files_file_open(name);
        IF id = 0 THEN RETURN NIL END;
        NEW(f);
        f.name := name;
        f.onDisk := TRUE;
        f.id := id;
        IF root = NIL THEN root := f ELSE f.next := root; root := f END;
        RETURN f
    END Old;

END Files.