(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** The Files module implements the Oberon file system. *)
MODULE Files; (*NW 11.1.86 / 28.8.92 / pjm 17.04.97 / MG 24.04.2025 *)
IMPORT SYSTEM;

TYPE
    Handle = POINTER TO RECORD END;  (* Declare FILE* data type from C <stdio.h> library. *)
    File* = POINTER TO RECORD
                name: ARRAY 32 OF CHAR;
                handle: Handle;
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


    (* Declare `FILE *fopen( char *, char * )` function from C <stdio.h> library. *)
    PROCEDURE [ "C" ] fopen(name: ARRAY OF CHAR; mode: ARRAY OF CHAR): Handle; EXTERNAL [ "fopen" ];

    (* Declare `FILE *tmpfile( void )` function from C <stdio.h> library. *)
    PROCEDURE [ "C" ] tmpfile(): Handle; EXTERNAL [ "tmpfile" ];

    (* Declare `int fgetc( FILE* )` function from C <stdio.h> library.  *)
    PROCEDURE [ "C" ] fgetc(handle: Handle): BYTE; EXTERNAL [ "fgetc" ];

    (* Declare `int fputc ( int, FILE * )` function from C <stdio.h> library. *)
    PROCEDURE [ "C" ] fputc(x: BYTE; handle: Handle): INTEGER; EXTERNAL [ "fputc" ];

    (* Declare `int32_t feof( FILE * )` function from C <stdio.h> library. *)
    PROCEDURE [ "C" ] feof(handle: Handle): INTEGER; EXTERNAL [ "feof" ];

    (* Declare `int64_t ftell( FILE* )` function from C <stdio.h> library. *)
    PROCEDURE [ "C" ] ftell(handle: Handle): LONGINT; EXTERNAL [ "ftell" ];

    (* Declare `int fflush( FILE * )` function from C <stdio.h> library. *)
    PROCEDURE [ "C" ] fflush(handle: Handle): INTEGER; EXTERNAL [ "fflush" ];

    (* Declare `int fclose( FILE * )` function from C <stdio.h> library. *)
    PROCEDURE [ "C" ] fclose(handle: Handle): INTEGER; EXTERNAL [ "fclose" ];

    (* Declare `bool olang_files_fexist(const char* )` from Oberon runtime library. *)
    PROCEDURE [ "C" ] fexists(name: ARRAY OF CHAR): BOOLEAN; EXTERNAL [ "olang_files_fexists" ];

    (* Declare `int32_t olang_files_fregister(FILE *, const char * )` from Oberon runtime library. *)
    PROCEDURE [ "C" ] fregister(handle: Handle; name: ARRAY OF CHAR); EXTERNAL [ "olang_files_fregister" ];

    (* Declare `int32_t remove(const char * )` from C <stdio.h> library. *)
    PROCEDURE [ "C" ] fremove(name: ARRAY OF CHAR): INTEGER; EXTERNAL [ "remove" ];

    (* Declare `int32_t rename(const char *, const char * )` from C <stdio.h> library. *)
    PROCEDURE [ "C" ] frename(old, new: ARRAY OF CHAR): INTEGER; EXTERNAL [ "rename" ];

    (* Declare `int64_t olang_files_flength(FILE * )` from Oberon runtime library. *)
    PROCEDURE [ "C" ] flength(handle: Handle): LONGINT; EXTERNAL [ "olang_files_flength" ];

    (* Declare `bool olang_files_fseek(FILE*, int64_t)` from Oberon runtime library. *)
    PROCEDURE [ "C" ] fseek(handle: Handle; offset: LONGINT): BOOLEAN; EXTERNAL [ "olang_files_fseek" ];

    (* Declare `void olang_files_fdate(const char *, int64_t *, int64_t * )` from Oberon runtime library. *)
    PROCEDURE [ "C" ] fdate(name: ARRAY OF CHAR; VAR t, d: LONGINT); EXTERNAL [ "olang_files_fdate" ];


    PROCEDURE Insert(f: File);
    BEGIN
        IF root = NIL THEN root := f ELSE f.next := root; root := f END
    END Insert;

    PROCEDURE Remove(f: File);
    VAR ff: File;
    BEGIN
        IF f = root THEN
            root := f.next
        ELSE
            ff := root;
            WHILE ff # NIL DO
                IF ff.next = f THEN ff.next := f.next; RETURN END;
                ff := ff.next
            END
        END
    END Remove;

    PROCEDURE Search(name: ARRAY OF CHAR): File;
    VAR f: File;
    BEGIN
        f := root;
        WHILE f # NIL DO
            IF f.name = name THEN EXIT END;
            f := f.next
        END;
        RETURN f
    END Search;

    (** Open an existing file. The same file descriptor is returned if a file is opened multiple times. *)
    PROCEDURE Old*(name: ARRAY OF CHAR): File;
    VAR f: File;
        handle: Handle;
    BEGIN
        f := Search(name);
        IF f = NIL THEN
            (* IF ~fexists(name) THEN RETURN NIL END; *)
            handle := fopen(name, "r+b");
            IF handle = NIL THEN
                handle := fopen(name, "rb");
                IF (handle = NIL) & fexists(name) THEN handle := fopen(name, "ab") END
            END;
            IF handle = NIL THEN RETURN NIL END;
            NEW(f);
            f.name := name;
            f.handle := handle;
            f.registered := TRUE;
            f.modified := FALSE;
            f.next := NIL;
            Insert(f)
        END;
        RETURN f
    END Old;

    (** Creates a new file with the specified name. *)
    PROCEDURE New*(name: ARRAY OF CHAR): File;
    VAR f: File;
    BEGIN
        NEW(f);
        f.handle := tmpfile();
        f.name := name;
        f.registered := FALSE;
        f.modified := FALSE;
        f.next := NIL;
        RETURN f
    END New;

    (** Enters the file `f` into the directory together with the name provided in the operation `New` that created `f`.
        The file buffers are written back. Any existing mapping of this name to another file is overwritten. *)
    PROCEDURE Register*(f: File);
    BEGIN
        IF ~f.registered THEN fregister(f.handle, f.name) END
    END Register;

    (** Flushes the changes made to a file to disk. Register will automatically Close a file. *)
    PROCEDURE Close*(f: File);
    VAR ff: File;
    BEGIN
        ASSERT(f # NIL);
        IF f.registered & f.modified & (f.name # "") THEN fflush(f.handle) END;
        fclose(f.handle);
        f.handle := NIL;
        Remove(f)
    END Close;

    (** Resets the length of file f to 0. *)
    PROCEDURE Purge*(f: File);
    BEGIN
        ASSERT(f # NIL);
        Close(f);
        IF f.registered THEN
            f.handle := fopen(f.name, "w+b")
        ELSE
            f.handle := tmpfile()
        END;
        f.modified := FALSE
    END Purge;

    (** Deletes a (registered) file with the name `name`. In order to delete a file, the file must be closed.
        Variable parameter `res` contains the status of the operation: 0 indicates success. *)
    PROCEDURE Delete*(name: ARRAY OF CHAR; VAR res: INTEGER);
    VAR f: File;
    BEGIN
        IF Search(name) = NIL THEN
            res := fremove(name)
        ELSE
            res := 1
        END
    END Delete;

    (** Renames a (registered) file with name `old` to `new`. In order to rename a file, the file must be
        closed. Variable `res` contains the status of the operation: `0` indicates success. *)
    PROCEDURE Rename*(old, new: ARRAY OF CHAR; VAR res: INTEGER);
    VAR f: File;
    BEGIN
        IF Search(old) = NIL THEN
            res := frename(old, new)
        ELSE
            res := 1
        END
    END Rename;

    (** Returns the current length of a file. *)
    PROCEDURE Length*(f: File): LONGINT;
    BEGIN
        RETURN flength(f.handle)
    END Length;

    (** Returns the time `t` and date `d` of the last modification of file `f`.
        The encoding is:
            hour = t DIV 4096;
            minute = t DIV 64 MOD 64;
            second = t MOD 64;
            year = d DIV 512;
            month = d DIV 32 MOD 16;
            day = d MOD 32. *)
    PROCEDURE GetDate*(f: File; VAR t, d: LONGINT);
    BEGIN
        ASSERT(f # NIL);
        t := 0; d := 0;
        IF f.registered THEN fdate(f.name, t, d) END
    END GetDate;

    (** Returns the full name of a file. *)
    PROCEDURE GetName*(f: File; VAR name: ARRAY OF CHAR);
    BEGIN
      	name := f.name
    END GetName;

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
            IF fseek(r.file.handle, r.pos) THEN
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

    (** Reads n bytes into the given buffer. *)
    PROCEDURE ReadBytes*(VAR r: Rider; VAR data: ARRAY OF BYTE; n: LONGINT);
    VAR b: BYTE;
        i: LONGINT;
    BEGIN
        FOR i := 0 TO n - 1 DO
            Read(r, b); data[i] := b; IF r.res # 0 THEN RETURN END
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

    (** Reads an short integer value. *)
    PROCEDURE ReadSInt*(VAR r: Rider; VAR x: SHORTINT);
    VAR data: WORD;
        num: LONGINT;
    BEGIN
        ReadBytes(r, data, LEN(data));
        DecodeInt(num, data);
        x := SYSTEM.VAL(SHORTINT, num)
    END ReadSInt;

    (** Reads an integer value. *)
    PROCEDURE ReadInt*(VAR r: Rider; VAR x: INTEGER);
    VAR data: DWORD;
        num: LONGINT;
    BEGIN
        ReadBytes(r, data, LEN(data));
        DecodeInt(num, data);
        x := SYSTEM.VAL(INTEGER, num)
    END ReadInt;

    (* Reads a long integer value. *)
    PROCEDURE ReadLInt*(VAR r: Rider; VAR x: LONGINT);
    VAR data: QWORD;
    BEGIN
        ReadBytes(r, data, LEN(data));
        DecodeInt(x, data)
    END ReadLInt;

    (* Reads a real value. *)
    PROCEDURE ReadReal*(VAR r: Rider; VAR x: REAL);
    VAR num: INTEGER;
    BEGIN
        ReadInt(r, num);
        x := SYSTEM.VAL(REAL, num)
    END ReadReal;

    (* Reads a long real value. *)
    PROCEDURE ReadLReal*(VAR r: Rider; VAR x: LONGREAL);
    VAR num: LONGINT;
    BEGIN
        ReadLInt(r, num);
        x := SYSTEM.VAL(LONGREAL, num)
    END ReadLReal;

    (* Reads a set value. *)
    PROCEDURE ReadSet*(VAR r: Rider; VAR x: SET);
    VAR num: INTEGER;
    BEGIN
        ReadInt(r, num);
        x := SYSTEM.VAL(SET, num)
    END ReadSet;

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
            IF fseek(r.file.handle, r.pos) THEN
                r.res := 0; r.eof := FALSE;
                IF fputc(x, r.file.handle) = x THEN (* if a write is successful, `fputc` returns the written `char`. *)
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

    (** Writes n bytes from the given buffer. *)
    PROCEDURE WriteBytes*(VAR r: Rider; data: ARRAY OF BYTE; n: LONGINT);
    VAR i: LONGINT;
    BEGIN
        FOR i := 0 TO n - 1 DO Write(r, data[i]) END
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

    (** Writes an short integer value. *)
    PROCEDURE WriteSInt*(VAR r: Rider; x: SHORTINT);
    VAR data: WORD;
    BEGIN
        EncodeInt(x, data);
        WriteBytes(r, data, LEN(data))
    END WriteSInt;

    (** Writes an integer value. *)
    PROCEDURE WriteInt*(VAR r: Rider; x: INTEGER);
    VAR data: DWORD;
    BEGIN
        EncodeInt(x, data);
        WriteBytes(r, data, LEN(data))
    END WriteInt;

    (** Writes an long integer value. *)
    PROCEDURE WriteLInt*(VAR r: Rider; x: LONGINT);
    VAR data: QWORD;
    BEGIN
        EncodeInt(x, data);
        WriteBytes(r, data, LEN(data))
    END WriteLInt;

    (** Writes an real value. *)
    PROCEDURE WriteReal*(VAR r: Rider; x: REAL);
    VAR num: INTEGER;
    BEGIN
        num := SYSTEM.VAL(INTEGER, x);
        WriteInt(r, num)
    END WriteReal;

    (** Writes an long real value. *)
    PROCEDURE WriteLReal*(VAR r: Rider; x: LONGREAL);
    VAR num: LONGINT;
    BEGIN
        num := SYSTEM.VAL(LONGINT, x);
        WriteLInt(r, num)
    END WriteLReal;

    (** Writes a set value. *)
    PROCEDURE WriteSet*(VAR r: Rider; x: SET);
    VAR num: INTEGER;
    BEGIN
        num := SYSTEM.VAL(INTEGER, x);
        WriteInt(r, num)
    END WriteSet;

    (** Writes a Boolean value. *)
    PROCEDURE WriteBool*(VAR r: Rider; x: BOOLEAN);
    BEGIN
    	IF x THEN Write(r, 1) ELSE Write(r, 0) END
    END WriteBool;

BEGIN
    root := NIL
END Files.