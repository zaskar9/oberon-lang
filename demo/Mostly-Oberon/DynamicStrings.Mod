(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)

(**DynamicStrings.Mod provides an implementation of dynamic strings
using a linked list of records holding ARRAY OF CHAR.  *)
MODULE DynamicStrings;
  IMPORT Strings;

  CONST vSize = 128;
  TYPE
    DynamicString* = POINTER TO DynamicStringDesc;
    DynamicStringDesc* = RECORD 
      value : ARRAY vSize OF CHAR; 
      next : DynamicString 
    END;

  (** New* will initialize a new dynamic string settings `.value` to
  an empty string and `.next` to NIL. *)
  PROCEDURE New*(VAR str : DynamicString);
  BEGIN NEW(str);
    str.value := ""; 
    str.next := NIL;
  END New;


  (** Set* copies an `ARRAY OF CHAR` into a DynamicString. This requires 
   that we add and link additional records if the `source` is longer than 
   `vSize`. *)
  PROCEDURE Set*(VAR str : DynamicString; source : ARRAY OF CHAR); 
    VAR cur, next : DynamicString; tmp : ARRAY vSize OF CHAR; 
        i, l : INTEGER;
  BEGIN cur := str; cur.value := "";
    l := Strings.Length(source);
    i := 0; 
    WHILE i < l DO
      Strings.Extract(source, i, i + vSize, tmp);
      Strings.Append(tmp, cur.value);
      i := i + Strings.Length(tmp);
      IF (i < l) THEN
        IF cur.next = NIL THEN
          New(next); cur.next := next;
        END;
        cur := cur.next;
      END; 
    END;
  END Set;

  
  (** GetChar* returns the character in the dynamic string at the
      position indicated. The value of c will hold the 
      character found or 0X, the value ok will hold TRUE if
      character was found or FALSE if it wasn't found. *)
  PROCEDURE GetChar*(s : DynamicString; pos : INTEGER; VAR c : ARRAY OF CHAR);
    VAR i, j : INTEGER; cur : DynamicString; continue : BOOLEAN;
  BEGIN
    cur := s;
    i := 0;
    c := "";
    continue := TRUE;
    WHILE (cur # NIL) & (i <= pos) & continue DO
      FOR j := 0 TO Strings.Length(cur.value) DO
        IF i = pos THEN
          Strings.Extract(cur.value, i, 1, c);
          continue := FALSE;
        END;
        INC(i);
      END;
      cur := cur.next;
    END;
  END GetChar;

  (** SetChar* sets a char in the dynamic string as the position given.
      If the set was successful ok will be set TRUE or FALSE otherwise. *)
  PROCEDURE SetChar*(VAR s : DynamicString; pos : INTEGER; c : ARRAY OF CHAR);
    VAR i, j : INTEGER; cur : DynamicString; continue : BOOLEAN;
      val : ARRAY 2 OF CHAR;
  BEGIN
    cur := s;
    i := 0;
    (* Make sure we are only setting one character *)
    Strings.Extract(c, 0, 1, val);
    continue := TRUE;
    WHILE (cur # NIL) & (i <= pos) & continue DO
      FOR j := 0 TO Strings.Length(cur.value) DO
        IF i = pos THEN
          Strings.Replace(val, j, cur.value);
          continue := FALSE;
        END;
        INC(i);
      END;
      cur := cur.next;
    END;
  END SetChar;


  (** ToCharArray* will copy a dynamic string into a suitable sized 
      ARRAY OF CHAR. The value of `ok` is set TRUE on success, FALSE
      if truncatation is required. *)
  PROCEDURE ToCharArray*(str : DynamicString; 
                         VAR dest : ARRAY OF CHAR; 
                         VAR ok : BOOLEAN);
    VAR cur : DynamicString; i : INTEGER;
  BEGIN 
    ok := FALSE;
    cur := str; i := 0;
    WHILE cur # NIL DO
      i := i + Strings.Length(cur.value);
      Strings.Append(cur.value, dest);
      cur := cur.next;
    END;
    ok := (i = Strings.Length(dest));
  END ToCharArray;


  (** Append* appends a dynamic string, extra, to the destination
      dynamic string called dest.  Memory is allocated during
      copying of the appended string's records.
      *)
  PROCEDURE Append*(extra: DynamicString; VAR dest : DynamicString);
    VAR cur : DynamicString;  
  BEGIN
    (* Move to the end of the dest DynamicString *)
    cur := dest;
    WHILE cur.next # NIL DO cur := cur.next; END;
    (* Starting initial pointer of `extra` copy its records
       input new records created in `cur`. *)
    WHILE extra # NIL DO
      (* Create a new record *)
      NEW(cur.next);
      cur.next.value := "";
      cur.next.next := NIL;
      (* Copy extra.value into new record *)
      Strings.Extract(extra.value, 0, vSize, cur.next.value);
      (* Advance to next record for both cur and extra *)
      extra := extra.next;
      cur := cur.next;
    END;
  END Append;

  (** AppendCharArray* copies an ARRAY OF CHAR into a new dynamic
      string, then uses `Append` to append the new dynamic string
      to our dest dynamic string. *)
  PROCEDURE AppendCharArray*(extra: ARRAY OF CHAR; VAR dest : DynamicString);
    VAR extraStr : DynamicString;    
  BEGIN
    (* Convert our extra ARRAY OF CHAR into a DynamicString *)
    New(extraStr); Set(extraStr, extra);
    (* Now we can append. *)
    Append(extraStr, dest);
  END AppendCharArray;

  (** Length() returns an INTEGER indicating the total length for
      all the records making up the dynamic string. *)
  PROCEDURE Length*( source : DynamicString) : INTEGER;
    VAR cur : DynamicString; total : INTEGER;
  BEGIN
    total := 0;
    cur := source;
    WHILE cur # NIL DO
      total := total + Strings.Length(cur.value);
      cur := cur.next;
    END; 
    RETURN total
  END Length;

  (** Insert inserts a "extra" dynamic string into a destination dynamic
      string at a given position. *)
  PROCEDURE Insert*(extra : DynamicString; 
                    pos : INTEGER; 
                    VAR dest : DynamicString);
    VAR cur, rest : DynamicString;
        tmp : ARRAY vSize OF CHAR;
        i, splitPos : INTEGER; continue : BOOLEAN;
  BEGIN
    (* 1. Set `cur` to the start of our `dest` dynamic string *)
    cur := dest;

    (* 2. Move to the record which holds the split point *)
    i := 0;
    continue := (i < pos);
    WHILE continue DO
      i := i + Strings.Length(cur.value);
      continue := (i < pos);
      IF continue & (cur.next # NIL) THEN
        cur := cur.next;
      ELSE
        continue := FALSE;
      END;
    END;

    (* 3. Copy the characters in `cur.value` from relative split point to end of `.value` into a temporary `ARRAY OF CHAR` *)
    splitPos := pos MOD vSize;
    Strings.Extract(cur.value, splitPos, Strings.Length(cur.value), tmp);

    (* 4. Make a new record, `rest`, using the temporary `ARRAY OF CHAR` and update the value of `.next` to match that of `cur.next` *)
    New(rest); Set(rest, tmp);
    rest.next := cur.next;

    (* 5. Truncate `cur.value` at the relative split point *)
    i := splitPos;
    WHILE i < LEN(cur.value) DO
      cur.value[i] := 0X;
      INC(i);
    END;

    (* 6. Set `cur.next` to point to our `extra` dynamic string. *)
    cur.next := extra;

    (* 7. Move to the end of extra with `cur` *)
    WHILE cur.next # NIL DO cur := cur.next; END;

    (* 8. Set the `cur.next` to point at `rest` *)
    cur.next := rest;
  END Insert;

  (** InsertCharArray creates a new dynamic string from source
      and inserts it using the original `Insert` procedure for
      dynamic strings. *)
  PROCEDURE InsertCharArray*(source : ARRAY OF CHAR; 
                             pos : INTEGER; 
                             VAR dest : DynamicString);
    VAR extra : DynamicString;
  BEGIN
    New(extra); Set(extra, source);
    Insert(extra, pos, dest);
  END InsertCharArray;

  (** Delete replaces the characters in the dynamic string
      from pos until n unlinking any un-used records. The GC 
      should then beable to recover the memory assuming no 
      additional pointers. *)
  PROCEDURE Delete*(VAR source : DynamicString; pos, n : INTEGER);
  BEGIN
  END Delete;

  (** Replace replaces src in the destination at the given
      position.  It is the equivalent of dones in a delete
      followed by insert. *)  
  PROCEDURE Replace*(source: DynamicString; 
                     pos : INTEGER; 
                     VAR dest : DynamicString);
  BEGIN
    (* This procedure has be left for the reader to implement. *)
  END Replace;

  (** Extract *)
  PROCEDURE Extract*(source : DynamicString;
                     pos, n : INTEGER; 
                     VAR dest : DynamicString);
  BEGIN
    (* This procedure has be left for the reader to implement. *)
  END Extract;

  (** Pos *)
  PROCEDURE Pos*(pattern : DynamicString; pos : INTEGER) : INTEGER;
    VAR p : INTEGER;
  BEGIN
    (* This procedure has be left for the reader to implement. *)
    p := 0;
    RETURN p
  END Pos;

  (** Cap *)
  PROCEDURE Cap*(VAR s : DynamicString);
  BEGIN
    (* This procedure has be left for the reader to implement. *)
  END Cap;

END DynamicStrings.
