(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** The Files module implements the Oberon file system. *)
MODULE Files; (*NW 11.1.86 / 28.8.92 / pjm 17.04.97 / MG 24.04.2025 *)

TYPE
    Handle = POINTER TO RECORD END;  (* Declare FILE* data type from C <stdio.h> library. *)
    File* = POINTER TO RECORD
                name: ARRAY 32 OF CHAR;
                handle: Handle;
                t, d: LONGINT;
                registered, modified: BOOLEAN;
                next: File
            END;
    Rider* = RECORD                  (** Riders are the access mechanisms for files. *)
			    eof*: BOOLEAN;	     (** Rider has reached the end of the file. *)
				res*, pos: LONGINT;	 (** Rider operation result code. *)
				file: File
			END;

    WORD = ARRAY 2 OF BYTE;
	DWORD = ARRAY 4 OF BYTE;
	QWORD = ARRAY 8 OF BYTE;

VAR
    root: File;


    (* Declare `FILE *tmpfile( void )` function from C <stdio.h> library. *)
    PROCEDURE tmpfile(): Handle; EXTERN;

    (* Declare `int fgetc( FILE* )` function from C <stdio.h> library.  *)
    PROCEDURE fgetc(handle: Handle): BYTE; EXTERN;

    (* Declare `int fputc ( int, FILE * )` function from C <stdio.h> library. *)
    PROCEDURE fputc(x: BYTE; handle: Handle): INTEGER; EXTERN;

    (* Declare `int32_t feof( FILE * )` function from C <stdio.h> library. *)
    PROCEDURE feof(handle: Handle): INTEGER; EXTERN;

    (* Declare `int64_t ftell( FILE* )` function from C <stdio.h> library. *)
    PROCEDURE ftell(handle: Handle): LONGINT; EXTERN;

    (* Declare `int fflush( FILE * )` function from C <stdio.h> library. *)
    PROCEDURE fflush(handle: Handle): INTEGER; EXTERN;

    (* Declare `int fclose( FILE * )` function from C <stdio.h> library. *)
    PROCEDURE fclose(handle: Handle): INTEGER; EXTERN;

    (* Declare `bool olang_files_file_exist(const char*, const void * )` from Oberon runtime library. *)
    PROCEDURE olang_files_file_exists(name: ARRAY OF CHAR): BOOLEAN; EXTERN;

    (* Declare `FILE *olang_files_file_exist(const char*, const void * )` from Oberon runtime library. *)
    PROCEDURE olang_files_file_open(name: ARRAY OF CHAR): Handle; EXTERN;

    (* Declare `int32_t olang_files_file_register(FILE *, const char *, const void * )` from Oberon runtime library. *)
    PROCEDURE olang_files_file_register(handle: Handle; name: ARRAY OF CHAR): INTEGER; EXTERN;

    (* Declare `bool olang_files_file_remove(const char *, const void * )` from Oberon runtime library. *)
    PROCEDURE olang_files_file_remove(name: ARRAY OF CHAR): BOOLEAN; EXTERN;

    (* Declare `bool olang_files_file_rename(const char *, const void *, const char *, const void* )` from Oberon runtime library. *)
    PROCEDURE olang_files_file_rename(old, new: ARRAY OF CHAR): BOOLEAN; EXTERN;

    (* Delcare `int64_t olang_files_file_length(FILE * )` from Oberon runtime library. *)
    PROCEDURE olang_files_file_length(handle: Handle): LONGINT; EXTERN;

    (* Declare `bool olang_files_file_seek(FILE*, int64_t)` from Oberon runtime library. *)
    PROCEDURE olang_files_file_seek(handle: Handle; offset: LONGINT): BOOLEAN; EXTERN;


    (** Creates a new file with the specified name. *)
    PROCEDURE New*(name: ARRAY OF CHAR): File;
    VAR f: File;
    BEGIN
        NEW(f);
        f.handle := tmpfile();
        f.name := name;
        f.registered := FALSE;
        f.modified := FALSE;
        RETURN f
    END New;

    (** Open an existing file. The same file descriptor is returned if a file is opened multiple times. *)
    PROCEDURE Old*(name: ARRAY OF CHAR): File;
    VAR f: File;
        handle: Handle;
    BEGIN
        f := root;
        WHILE f # NIL DO
          IF f.name = name THEN RETURN f END;
          f := f.next
        END;
        IF ~olang_files_file_exists(name) THEN RETURN NIL END;
        handle := olang_files_file_open(name);
        IF handle = NIL THEN RETURN NIL END;
        NEW(f);
        f.name := name;
        f.handle := handle;
        f.registered := TRUE;
        f.modified := FALSE;
        IF root = NIL THEN root := f ELSE f.next := root; root := f END;
        RETURN f
    END Old;

    (** Flushes the changes made to a file to disk. Register will automatically Close a file. *)
    PROCEDURE Close*(f: File);
    VAR ff: File;
    BEGIN
        IF f.registered & f.modified & (f.name # "") THEN fflush(f.handle) END;
        fclose(f.handle);
        f.handle := NIL;
        ff := root;
        WHILE ff # NIL DO
            IF ff.next = f THEN ff.next := f.next; RETURN END;
            ff := ff.next
        END;
    END Close;

    (** Register a file created with New in the directory, replacing the previous file in the
        directory with the same name. The file is automatically closed. *)
    PROCEDURE Register*(f: File);
    VAR res: BOOLEAN;
    BEGIN
        IF ~f.registered THEN olang_files_file_register(f.handle, f.name) END;
        Close(f)
    END Register;

    (** Deletes a file. res = 0 indicates success. *)
    PROCEDURE Delete*(name: ARRAY OF CHAR; VAR res: INTEGER);
    VAR f: File;
    BEGIN
        IF olang_files_file_remove(name) THEN
            res := 0;
            f := root;
            WHILE f # NIL DO
                IF f.name = name THEN Close(f); RETURN END;
                f := f.next
            END
        ELSE
            res := 1
        END
    END Delete;

    (** Renames a file. res = 0 indicates success. *)
    PROCEDURE Rename*(old, new: ARRAY OF CHAR; VAR res: INTEGER);
    VAR f: File;
    BEGIN
        IF olang_files_file_rename(old, new) THEN
            res := 0;
            f := root;
            WHILE f # NIL DO
                IF f.name = old THEN f.name := new; RETURN END;
                f := f.next
            END
        ELSE
            res := 1
        END
    END Rename;

    (** Returns the full name of a file. *)
    PROCEDURE GetName*(f: File; VAR name: ARRAY OF CHAR);
    BEGIN
      	name := f.name
    END GetName;

    (** Returns the current length of a file. *)
    PROCEDURE Length*(f: File): LONGINT;
    BEGIN
        RETURN olang_files_file_length(f.handle)
    END Length;

    (** Positions a Rider at a certain position in a file. Multiple Riders can be positioned at
        different locations in a file. A Rider cannot be positioned beyond the end of a file. *)
    PROCEDURE Set*(VAR r: Rider; f: File; pos: LONGINT);
    VAR a, b: INTEGER;
    BEGIN
        r.eof := FALSE; r.res := 0;
        IF f # NIL THEN
            r.file := f; r.pos := pos
        ELSE
            r.file:= NIL
        END
    END Set;

    (** Returns the offset of a Rider positioned on a file. *)
    PROCEDURE Pos*(VAR r: Rider): LONGINT;
    BEGIN
        RETURN r.pos
    END Pos;

    (** Returns the File a Rider is based on. *)
    PROCEDURE Base*(VAR r: Rider): File;
    BEGIN
        RETURN r.file
    END Base;

    (** Read a byte from a file, advancing the Rider one byte further. The value of `r.eof`
        indicates if the end of the file has been passed. *)
	PROCEDURE Read*(VAR r: Rider; VAR x: BYTE);
	VAR pos: LONGINT;
    BEGIN
        x := 0;
        IF r.file # NIL THEN
            IF olang_files_file_seek(r.file.handle, r.pos) THEN
                r.res := 0; r.eof := FALSE;
                IF feof(r.file.handle) # 0 THEN
                    r.eof := TRUE; r.res := 1
                ELSE
                    x := fgetc(r.file.handle);
                    pos := ftell(r.file.handle);
                    IF pos = r.pos THEN r.res := 1 ELSE r.pos := pos END
                END
            ELSE
                r.res := 1
            END
        END
    END Read;

    (** Read a zero-terminated string. *)
    PROCEDURE ReadString*(VAR R: Rider; VAR x: ARRAY OF CHAR);
    VAR i: INTEGER; ch: BYTE;
    BEGIN
        i := 0;
        LOOP
            Read(R, ch); x[i] := CHR(ch); INC(i);
            IF ch = 0 THEN EXIT END;
            IF i = LEN(x) THEN
                x[i-1] := 0X;
                REPEAT Read(R, ch) UNTIL ch = 0;
                EXIT
            END
        END
    END ReadString;

    (** Writes a byte into the file at the Rider position, advancing the Rider by one. *)
    PROCEDURE Write*(VAR r: Rider; x: BYTE);
    VAR pos: LONGINT;
    BEGIN
        IF r.file # NIL THEN
            IF olang_files_file_seek(r.file.handle, r.pos) THEN
                r.res := 0; r.eof := FALSE;
                IF fputc(x, r.file.handle) # 0 THEN
                    r.file.modified := TRUE;
                    pos := ftell(r.file.handle);
                    IF pos = r.pos THEN r.res := r.res + 1 ELSE r.pos := pos END
                ELSE
                    r.res := r.res + 1
                END
            ELSE
                r.res := r.res + 1
            END
        END
    END Write;

    (** Writes a zero-terminated string. *)
    PROCEDURE WriteString*(VAR R: Rider; x: ARRAY OF CHAR);
    VAR i: INTEGER; ch: BYTE;
    BEGIN
        i := 0;
        LOOP ch := ORD(x[i]); Write(R, ch); INC(i);
            IF ch = 0 THEN EXIT END;
            IF i = LEN(x) THEN Write(R, 0); EXIT END
        END
    END WriteString;

END Files.