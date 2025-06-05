(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** The Files module implements the Oberon file system. *)
MODULE Files; (*NW 11.1.86 / 28.8.92 / pjm 17.04.97 / MG 24.04.2025 *)
IMPORT Out, SYSTEM;

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

    (* Declare `int64_t olang_files_file_length(FILE * )` from Oberon runtime library. *)
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
        END
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

    (** Reads a byte from a file, advancing the Rider one byte further. The value of `r.eof`
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
                    IF pos = r.pos THEN x := 0; r.res := 1 ELSE r.pos := pos END
                END
            ELSE
                r.res := 1
            END
        END
    END Read;

    PROCEDURE ReadBytes*(VAR r: Rider; VAR data: ARRAY OF BYTE);
    VAR b: BYTE;
        i: LONGINT;
    BEGIN
        FOR i := 0 TO LEN(data) - 1 DO
            Read(r, b); data[i] := b; IF r.res # 0 THEN RETURN END;
            Out.Int(data[i], 4)
        END
    END ReadBytes;

    (** Reads a character. *)
    PROCEDURE ReadChar*(VAR r: Rider; VAR x: CHAR);
    VAR b: BYTE;
    BEGIN
        Read(r, b); x := CHR(b)
    END ReadChar;

    (** Reads a zero-terminated string. *)
    PROCEDURE ReadString*(VAR r: Rider; VAR x: ARRAY OF CHAR);
    VAR i: INTEGER; ch: BYTE;
    BEGIN
        i := 0;
        LOOP
            Read(r, ch); x[i] := CHR(ch); INC(i);
            IF ch = 0 THEN EXIT END;
            IF i = LEN(x) THEN
                x[i - 1] := 0X;
                REPEAT Read(r, ch) UNTIL ch = 0;
                EXIT
            END
        END
    END ReadString;

    (** Reads a number in compressed variable length notation using the minimum amount of bytes. *)
    PROCEDURE ReadNum*(VAR r: Rider; VAR x: LONGINT);
    VAR b: BYTE; s: INTEGER; ch, n, y, z: LONGINT;
    BEGIN
      	n := 0; s := 0; Read(r, b); ch := b;
    	WHILE ch >= 128 DO
      		INC(n, LSL(ch - 128, s));
      		INC(s, 7);
      		Read(r, b); ch := b
      	END;
      	y := ch MOD 64 - ch DIV 64 * 64;
      	IF y < 0 THEN z := -LSL(-y, s) ELSE z := LSL(y, s) END;
        x := n + z
    END ReadNum;

    PROCEDURE DecodeInt(VAR num: LONGINT; data: ARRAY OF BYTE);
    VAR i: LONGINT;
    BEGIN
        num := 0;
        FOR i := 0 TO LEN(data) - 1 DO
            num := num + LSL(data[i], i * 8)
        END
    END DecodeInt;

    (** Reads an integer value. *)
    PROCEDURE ReadInt*(VAR r: Rider; VAR x: INTEGER);
    VAR data: DWORD;
        num: LONGINT;
    BEGIN
        ReadBytes(r, data);
        DecodeInt(num, data);
        x := SYSTEM.VAL(INTEGER, num)
    END ReadInt;

    (** Reads a Boolean value. *)
    PROCEDURE ReadBool*(VAR r: Rider; VAR x: BOOLEAN);
    VAR b: BYTE;
    BEGIN
        Read(r, b); x := b # 0
    END ReadBool;

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

    PROCEDURE WriteBytes*(VAR r: Rider; data: ARRAY OF BYTE);
    VAR i: LONGINT;
    BEGIN
        FOR i := 0 TO LEN(data) - 1 DO Write(r, data[i]) END
    END WriteBytes;

    (** Writes a character. *)
    PROCEDURE WriteChar*(VAR r: Rider; x: CHAR);
    BEGIN
      	Write(r, ORD(x))
    END WriteChar;

    (** Writes a zero-terminated string. *)
    PROCEDURE WriteString*(VAR r: Rider; x: ARRAY OF CHAR);
    VAR i: INTEGER; ch: BYTE;
    BEGIN
        i := 0;
        LOOP ch := ORD(x[i]); Write(r, ch); INC(i);
            IF ch = 0 THEN EXIT END;
            IF i = LEN(x) THEN Write(r, 0); EXIT END
        END
    END WriteString;

    (** Writes a number in a compressed format. *)
    PROCEDURE WriteNum*(VAR r: Rider; x: LONGINT);
    BEGIN
        WHILE (x < -64) OR (x > 63) DO
            Write(r, x MOD 128 + 128);
            x := x DIV 128
        END;
        Write(r, x MOD 128)
    END WriteNum;

    PROCEDURE EncodeInt(num: LONGINT; VAR data: ARRAY OF BYTE);
    VAR i: LONGINT;
    BEGIN
        FOR i := 0 TO LEN(data) - 1 DO
            data[i] := num MOD 100H;
            num := num DIV 100H
        END
    END EncodeInt;

    (** Writes an integer. *)
    PROCEDURE WriteInt*(VAR r: Rider; x: INTEGER);
    VAR data: DWORD;
    BEGIN
        EncodeInt(x, data);
        WriteBytes(r, data)
    END WriteInt;

    (** Writes a Boolean value. *)
    PROCEDURE WriteBool*(VAR r: Rider; x: BOOLEAN);
    BEGIN
    	IF x THEN Write(r, 1) ELSE Write(r, 0) END
    END WriteBool;

END Files.