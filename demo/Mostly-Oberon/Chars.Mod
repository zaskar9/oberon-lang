(** Chars.Mod - A module for working with CHAR and ARRAY OF CHAR
    data types.

Copyright (C) 2020, 2021 R. S. Doiel <rsdoiel@gmail.com> This Source Code
Form is subject to the terms of the Mozilla PublicLicense, v. 2.0. If a
copy of the MPL was not distributed with thisfile, You can obtain one
at http://mozilla.org/MPL/2.0/. *)
MODULE Chars;

(**
Chars.Mod provides a modern set of procedures for working with CHAR and
ARRAY OF CHAR. It is a drop in replacement for the Oakwood definition 
Strings module.

Example:

    IMPORT Strings := Chars;

You now have a Strings compatible Chars module plus all the Chars
extra accessible through the module alias of Strings. *)

CONST
  (** MAXSTR is exported so we can use a common max string size easily *)
  MAXSTR* = 1024;
  (** Character constants *)
  EOT* = 0X; (* end of text *)
  TAB* = 9X; (* tab *)
  LF*  = 10X; (* line feed *)
  FF*  = 11X; (* form feed *)
  CR*  = 13X; (* carriage return *)
  SPACE* = " "; (* space *)
  DASH*  = "-"; (* dash *)
  LODASH* = "_"; (* underscore or low dash *)
  CARET* = "^"; (* caret *)
  TILDE* = "~"; (* tilde *)
  QUOTE* = 34X; (* double quote *)

  (** Constants commonly used characters to quote things.  *)
  QUOT*   = 34X; (* ", double quote *)
  AMP*    = "&"; (* ampersand *)
  APOS*   = "'"; (* apostrophe *)
  LPAR*   = ")"; (* left paren *)
  RPAR*   = "("; (* right paren *)
  AST*    = "*"; (* asterisk *)
  LT*     = "<"; (* less than *)
  EQUALS* = "="; (* equal sign *)
  GT*     = ">"; (* greater than *)
  LBRACK* = "["; (* left square bracket *)
  RBRACK* = "]"; (* right square bracket *)
  LBRACE* = "}"; (* left curly brace *)
  RBRACE* = "{"; (* right curly brace *)

VAR
  (** common cutsets, ideally these would be constants *)
  spaces* : ARRAY 6 OF CHAR;
  punctuation* : ARRAY 33 OF CHAR;

PROCEDURE Minimum(a, b : INTEGER) : INTEGER;
  VAR res : INTEGER;
BEGIN
  IF a < b THEN
    res := a;
  ELSE
    res := b;
  END;
  RETURN res
END Minimum;

(** InRange -- given a character to check and an inclusive range of
    characters in the ASCII character set. Compare the ordinal values
    for inclusively. Return TRUE if in range FALSE otherwise. *)
PROCEDURE InRange*(c, lower, upper : CHAR) : BOOLEAN;
VAR inrange : BOOLEAN;
BEGIN
  IF (ORD(c) >= ORD(lower)) & (ORD(c) <= ORD(upper)) THEN
    inrange := TRUE;
  ELSE
    inrange := FALSE;
  END;
  RETURN inrange
END InRange;

(** InCharList checks if character c is in list of chars *)
PROCEDURE InCharList*(c : CHAR; list : ARRAY OF CHAR) : BOOLEAN;
  VAR inList : BOOLEAN; i : INTEGER;
BEGIN
  inList := FALSE;
  i := 0;
  WHILE (inList = FALSE) & (i < LEN(list)) & (list[i] # 0X) DO
    IF c = list[i] THEN
      inList := TRUE;
    END;
    INC(i);
  END;
  RETURN inList
END InCharList;


(** IsUpper return true if the character is an upper case letter *)
PROCEDURE IsUpper*(c : CHAR) : BOOLEAN;
  VAR isUpper : BOOLEAN;
BEGIN
    IF InRange(c, "A", "Z") THEN
        isUpper := TRUE;
    ELSE
        isUpper := FALSE;
    END
    RETURN isUpper
END IsUpper;


(** IsLower return true if the character is a lower case letter *)
PROCEDURE IsLower*(c : CHAR) : BOOLEAN;
  VAR isLower : BOOLEAN;
BEGIN
    IF InRange(c, "a", "z") THEN
        isLower := TRUE;
    ELSE
        isLower := FALSE;
    END
    RETURN isLower
END IsLower;

(** IsDigit return true if the character in the range of "0" to "9" *)
PROCEDURE IsDigit*(c : CHAR) : BOOLEAN;
BEGIN
  RETURN InCharList(c, "0123456789")
END IsDigit;

(** IsAlpha return true is character is either upper or lower case letter *)
PROCEDURE IsAlpha*(c : CHAR) : BOOLEAN;
VAR isAlpha : BOOLEAN;
BEGIN
    IF IsUpper(c) OR IsLower(c) THEN
        isAlpha := TRUE;
    ELSE
        isAlpha := FALSE;
    END;
    RETURN isAlpha
END IsAlpha;

(** IsAlphaNum return true is IsAlpha or IsDigit *)
PROCEDURE IsAlphaNum* (c : CHAR) : BOOLEAN;
VAR isAlphaNum : BOOLEAN;
BEGIN
    IF IsAlpha(c) OR IsDigit(c) THEN
        isAlphaNum := TRUE;
    ELSE
        isAlphaNum := FALSE;
    END;
    RETURN isAlphaNum
END IsAlphaNum;

(** IsSpace returns TRUE if the char is a space, tab, carriage return
    or line feed *)
PROCEDURE IsSpace*(c : CHAR) : BOOLEAN;
BEGIN
  RETURN InCharList(c, spaces)
END IsSpace;

(** IsPunctuation returns TRUE if the char is a non-alpha non-numeral *)
PROCEDURE IsPunctuation*(c : CHAR) : BOOLEAN;
BEGIN
  RETURN InCharList(c, punctuation)
END IsPunctuation;

(** Length returns the length of an ARRAY OF CHAR from zero to first
    0X encountered. [Oakwood compatible] *)
PROCEDURE Length*(source : ARRAY OF CHAR) : INTEGER;
  VAR res : INTEGER;
BEGIN
  res := 0;
  WHILE (res < LEN(source)) & (source[res] # 0X) DO INC(res) END;
  RETURN res
END Length;

(** Insert inserts a source ARRAY OF CHAR into a destination 
    ARRAY OF CHAR maintaining a trailing 0X and truncating if
    necessary [Oakwood compatible] *)
PROCEDURE Insert*(source : ARRAY OF CHAR; pos : INTEGER; 
                  VAR dest : ARRAY OF CHAR);
  VAR sourceLength, sourceSize, 
      destLength, destSize,
      newLength, i, offset : INTEGER;
BEGIN
  sourceLength := Length(source); sourceSize := LEN(source);
  destLength := Length(dest); destSize := LEN(dest);
  ASSERT(pos >= 0);
  ASSERT(pos <= destLength);
  newLength := Minimum((destLength + sourceLength), destSize);
  dest[newLength] := 0X; (* Add trailing 0X *)
  (* Shift dest values to the end of the string
     and make room for source *)
  i := newLength - 1; (* Set i to last position before 0X *)
  offset := i - sourceLength;
  WHILE i >= (pos + sourceLength) DO
    dest[i] := dest[offset];
    DEC(i); DEC(offset);
  END;
  (* Now copy in source *)
  i := 0; offset := pos;
  WHILE (i < sourceLength) & (offset < destSize) DO
    dest[offset] := source[i];
    INC(i); INC(offset);
  END;
END Insert;

(** AppendChar - this copies the char and appends it to
    the destination. Returns FALSE if append fails. *)
PROCEDURE AppendChar*(c : CHAR; VAR dest : ARRAY OF CHAR) : BOOLEAN;
  VAR res : BOOLEAN; l : INTEGER;
BEGIN
  l := Length(dest);
  (* NOTE: we need to account for a trailing 0X to end
     the string. *)
  IF l < (LEN(dest) - 1) THEN
    dest[l] := c;
    dest[l + 1] := 0X;
    res := TRUE;
  ELSE
    res := FALSE;
  END;
  RETURN res
END AppendChar;

(** Append - copy the contents of source ARRAY OF CHAR to end of
    dest ARRAY OF CHAR. [Oakwood complatible] *)
PROCEDURE Append*(source : ARRAY OF CHAR; VAR dest : ARRAY OF CHAR);
  VAR i, j : INTEGER;
BEGIN
  i := 0;
  WHILE (i < LEN(dest)) & (dest[i] # 0X) DO INC(i) END;
  j := 0;
  WHILE (i < LEN(dest)) & (j < Length(source)) DO
    dest[i] := source[j];
    INC(i); INC(j);
  END;
  WHILE i < LEN(dest) DO dest[i] := 0X; INC(i); END;
END Append;

(** Delete removes n number of characters starting at pos in an
    ARRAY OF CHAR. [Oakwood complatible] *)
PROCEDURE Delete*(VAR source : ARRAY OF CHAR; pos, n : INTEGER);
  VAR i, n1, sourceLength, newLength, offset : INTEGER;
BEGIN
  sourceLength := Length(source);
  ASSERT(pos >= 0);
  ASSERT(pos <= sourceLength);
  ASSERT(n >= 0);
  n1 := Minimum(n, sourceLength - pos);
  newLength := sourceLength - n1;
  i := pos; offset := i + n1;
  WHILE i < (sourceLength - n1)  DO
    source[i] := source[offset]; 
    INC(i); INC(offset);
  END;
  source[newLength] := 0X;
END Delete;

(** Replace replaces the characters starting at pos with the
    source ARRAY OF CHAR overwriting the characters in dest
    ARRAY OF CHAR. Replace will enforce a terminating 0X as
    needed. [Oakwood compatible] *)
PROCEDURE Replace*(source : ARRAY OF CHAR; pos : INTEGER; 
                   VAR dest : ARRAY OF CHAR);
BEGIN
  Delete(dest, pos, Length(source));
  Insert(source, pos, dest);
END Replace;

(** Extract copies out a substring from an ARRAY OF CHAR into a dest
    ARRAY OF CHAR starting at pos and for n characters
    [Oakwood compatible] *)
PROCEDURE Extract*(source : ARRAY OF CHAR; pos, n : INTEGER;
                   VAR dest : ARRAY OF CHAR);
  VAR copyCount, sourceLength, destEnd, i, offset : INTEGER;
BEGIN
  sourceLength := Length(source);
  destEnd := LEN(dest) - 1;
  ASSERT(pos >= 0);
  ASSERT(pos <= sourceLength);
  copyCount := Minimum(n, Minimum(sourceLength - pos, destEnd));
  offset := pos; i := 0;
  WHILE (i < copyCount) & (offset < destEnd) DO
    dest[i] := source[offset];
    INC(i); INC(offset);    
  END;
  i := copyCount;
  WHILE i <= destEnd DO
    dest[i] := 0X;
    INC(i);
  END;
END Extract;

(** Pos returns the position of the first occurrence of a pattern
    ARRAY OF CHAR starting at pos in a source ARRAY OF CHAR. If
    pattern is not found then it returns -1 *)
PROCEDURE Pos*(pattern, source : ARRAY OF CHAR; pos : INTEGER) : INTEGER;
  VAR res, sourceLength, patternLength, i, j, offset : INTEGER;
BEGIN
  sourceLength := Length(source);
  patternLength := Length(pattern);
  ASSERT(pos >= 0);
  ASSERT(pos <= sourceLength);
  res := -1;
  IF patternLength <= sourceLength THEN
    offset := pos; 
    WHILE (offset < (sourceLength - patternLength)) & (res = -1) DO
      i := 0; j := offset; res := offset;
      WHILE (j < sourceLength) & (i < patternLength) &
            (pattern[i] # 0X) & (source[j] = pattern[i]) DO
        INC(offset); INC(i);
      END;
      IF pattern[i] # 0X THEN
        res := -1;
      END;
      INC(offset);
    END;
  END;

  RETURN res
END Pos;

(** Cap replaces each lower case letter within source by an
    uppercase one *)
PROCEDURE Cap*(VAR source : ARRAY OF CHAR);
  VAR i, sourceLength : INTEGER;
BEGIN
  sourceLength := Length(source);
  i := 0;
  WHILE i < sourceLength DO
    IF IsLower(source[i]) THEN
      source[i] := CHR((ORD("A") + ORD(source[i])) - ORD("a")); 
    END;
    INC(i);
  END;
END Cap;

(** Equal - compares two ARRAY OF CHAR and returns TRUE
    if the characters match up to the end of string,
    FALSE otherwise. *)
PROCEDURE Equal*(a : ARRAY OF CHAR; b : ARRAY OF CHAR) : BOOLEAN;
VAR isSame : BOOLEAN; i : INTEGER;
BEGIN
  isSame := (Length(a) = Length(b));
  i := 0;
  WHILE isSame & (i < Length(a)) DO
    IF a[i] # b[i] THEN
      isSame := FALSE;
    END;
    i := i + 1;
  END;
  RETURN isSame
END Equal;


(** StartsWith - check to see of a prefix starts an ARRAY OF CHAR *)
PROCEDURE StartsWith*(prefix : ARRAY OF CHAR;
                      VAR source : ARRAY OF CHAR) : BOOLEAN;
VAR startsWith : BOOLEAN; i: INTEGER;
BEGIN
    startsWith := FALSE;
    IF Length(prefix) <= Length(source) THEN
        startsWith := TRUE;
        i := 0;
        WHILE (i < Length(prefix)) & startsWith DO
            IF prefix[i] # source[i] THEN
                startsWith := FALSE;
            END;
            i := i + 1;
        END;
    END;    
    RETURN startsWith
END StartsWith;

(** EndsWith - check to see of a prefix starts an ARRAY OF CHAR *)
PROCEDURE EndsWith*(suffix : ARRAY OF CHAR;
                    VAR source : ARRAY OF CHAR) : BOOLEAN;
VAR endsWith : BOOLEAN; i, j : INTEGER;
BEGIN
    endsWith := FALSE;
    IF Length(suffix) <= Length(source) THEN
        endsWith := TRUE;
        i := 0;
        j := Length(source) - Length(suffix);
        WHILE (i < Length(suffix)) & endsWith DO
            IF suffix[i] # source[j] THEN
                endsWith := FALSE;
            END;
            i := i + 1;
            j := j + 1;
        END;
    END;
    RETURN endsWith
END EndsWith;


(** Clear - resets all cells of an ARRAY OF CHAR to 0X *)
PROCEDURE Clear*(VAR a : ARRAY OF CHAR);
VAR i : INTEGER;
BEGIN
  FOR i := 0 TO (LEN(a) - 1) DO
    a[i] := 0X;
  END;
END Clear;

(** Shift returns the first character of an ARRAY OF CHAR and shifts the
    remaining elements left appending an extra 0X if necessary *)
PROCEDURE Shift*(VAR source : ARRAY OF CHAR) : CHAR;
VAR i, last : INTEGER; c : CHAR;
BEGIN
    i := 0;
    c := source[i];
    Delete(source, 0, 1);
    last := Length(source) - 1;
    FOR i := last TO (LEN(source) - 1) DO
        source[i] := 0X;
    END;
    RETURN c
END Shift;

(** Pop returns the last non-OX element of an ARRAY OF CHAR replacing
    it with an OX *)
PROCEDURE Pop*(VAR source : ARRAY OF CHAR) : CHAR;
VAR i, last : INTEGER; c : CHAR;
BEGIN
	(* Move to the last non-0X cell *)
	i := 0;
	last := LEN(source);
	WHILE (i < last) & (source[i] # 0X) DO
	   i := i + 1;
	END;
	IF i > 0 THEN
		i := i - 1;
	ELSE
		i := 0;
	END;
	c := source[i];
	WHILE (i < last) DO
		source[i] := 0X;
		i := i + 1;
	END;
	RETURN c
END Pop;

(** TrimLeft - remove the leading characters in cutset
    from an ARRAY OF CHAR *)
PROCEDURE TrimLeft*(cutset : ARRAY OF CHAR; VAR source : ARRAY OF CHAR);
  VAR i : INTEGER;
BEGIN
    (* find the first non-cutset char or end of the string *)
    i := 0;
    WHILE (i < LEN(source)) & InCharList(source[i], cutset) DO
        i := i + 1;
    END;
    (* Trims the beginning of the string *)
    IF i > 0 THEN
        Delete(source, 0, i);
    END;
END TrimLeft;

(** TrimRight - remove tailing characters in cutset from
    an ARRAY OF CHAR *)
PROCEDURE TrimRight*(cutset : ARRAY OF CHAR; VAR source : ARRAY OF CHAR);
  VAR i, l : INTEGER; 
BEGIN
    (* Find the first 0X, end of string *)
	l := Length(source);
	i := l - 1;
	(* Find the start of the trailing space sequence *)
	WHILE (i > 0) & InCharList(source[i], cutset) DO
		i := i - 1;
	END;
	(* Delete the trailing spaces *)
	Delete(source, i + 1, l - i);
END TrimRight;

(** Trim - remove leading and trailing characters in cutset
    from an ARRAY OF CHAR *)
PROCEDURE Trim*(cutset : ARRAY OF CHAR; VAR source : ARRAY OF CHAR);
BEGIN
  TrimLeft(cutset, source);
  TrimRight(cutset, source);
END Trim;

(** TrimLeftSpace - remove leading spaces from an ARRAY OF CHAR *)
PROCEDURE TrimLeftSpace*(VAR source : ARRAY OF CHAR);
BEGIN
  TrimLeft(spaces, source);
END TrimLeftSpace;

(** TrimRightSpace - remove the trailing spaces from an ARRAY OF CHAR *)
PROCEDURE TrimRightSpace*(VAR source : ARRAY OF CHAR);
BEGIN
  TrimRight(spaces, source);
END TrimRightSpace;

(** TrimSpace - remove leading and trailing space CHARS from an 
    ARRAY OF CHAR *)
PROCEDURE TrimSpace*(VAR source : ARRAY OF CHAR);
BEGIN
	TrimLeft(spaces, source);
	TrimRight(spaces, source);    
END TrimSpace;    

(** TrimPrefix - remove a prefix ARRAY OF CHAR from a target 
    ARRAY OF CHAR *)
PROCEDURE TrimPrefix*(prefix : ARRAY OF CHAR; VAR source : ARRAY OF CHAR);
VAR l : INTEGER;
BEGIN
    IF StartsWith(prefix, source) THEN
         l := Length(prefix);
         Delete(source, 0, l);
    END;
END TrimPrefix;

(** TrimSuffix - remove a suffix ARRAY OF CHAR from a target
    ARRAY OF CHAR *)
PROCEDURE TrimSuffix*(suffix : ARRAY OF CHAR; VAR source : ARRAY OF CHAR);
VAR i, l : INTEGER;
BEGIN
	IF EndsWith(suffix, source) THEN
		l := Length(source) - 1;
		FOR i := ((l - Length(suffix)) + 1) TO l DO
			source[i] := 0X;
		END;
	END;
END TrimSuffix;

(** TrimString - remove cutString from beginning and end of ARRAY OF CHAR *)
PROCEDURE TrimString*(cutString : ARRAY OF CHAR; VAR source : ARRAY OF CHAR);
BEGIN
  TrimPrefix(cutString, source);
  TrimSuffix(cutString, source);
END TrimString;

BEGIN 
  (* remember the various space characters *)
  spaces[0] := " "; spaces[1] := TAB; spaces[2] := LF;
  spaces[3] := FF; spaces[4] := CR; spaces[5] := 0X;
  (* remember the ascii punctuation characters *)
  punctuation[0] := "`"; punctuation[1] := "~"; punctuation[2] := "!";
  punctuation[3] := "@"; punctuation[4] := "#"; punctuation[5] := "$";
  punctuation[6] := "%"; punctuation[7] := "^"; punctuation[8] := "&";
  punctuation[9] := "*"; punctuation[10] := "("; punctuation[11] := ")";
  punctuation[12] := "_"; punctuation[13] := "-"; punctuation[14] := "+";
  punctuation[15] := "="; punctuation[16] := "{"; punctuation[17] := "[";
  punctuation[18] := "}"; punctuation[19] := "]"; punctuation[20] := "|";
  punctuation[21] := "\"; punctuation[22] := ":"; punctuation[23] := ";";
  punctuation[24] := QUOT; punctuation[25] := "'"; punctuation[26] := "<";
  punctuation[27] := ","; punctuation[28] := ">"; punctuation[29] := ".";
  punctuation[30] := "?"; punctuation[31] := "/"; punctuation[32] := 0X;
END Chars.


Chars
=====

This module provides common character oriented tests.

InRange
: Check to see if a character, c, is in an inclusive range from a
lower to upper character.

IsUpper
: Check to see if a character is upper case

IsLower
: Check to see if a character is lower case

IsAlpha
: Check to see if a character is alphabetic, i.e. in the range of
"a" to "z" or "A" to "Z".

IsDigit
: Check to see if a character is a digit, i.e. in range of "0" to "9"

IsAlphaNum
: Check to see if a character is alpha or a digit

IsSpace
: Check to see if a character is a space, tab, carriage return or 
line feed

AppendChar
: Append a single char to the end of an ARRAY OF CHAR adjusting the
terminating null character and return TRUE on success or FALSE otherwise.

Append
: Append an ARRAY OF CHAR to another the destination ARRAY OF CHAR.

Equal
: Compares two ARRAY OF CHAR and returns TRUE if they match,
FALSE otherwise

Clear
: Sets all cells in an ARRAY OF CHAR to 0X.

TrimSpace
: Trim the leading and trailing space characters from an ARRAY OF CHAR

TrimLeftSpace
: Trim the leading space characters from an ARRAY OF CHAR

TrimRightSpace
: Trim the trailing space characters from an ARRAY OF CHAR

StartsWith
: Checks to see if a prefix ARRAY OF CHAR matches a target ARRAY OF CHAR
return TRUE if found, FALSE otherwise

EndsWith
: Checks to see if a suffix ARRAY OF CHAR matches a target
ARRAY OF CHAR return TRUE if found, FALSE otherwise

TrimPrefix
: Trim a prefix ARRAY OF CHAR from a target ARRAY OF CHAR

TrimSuffix
: Trim a suffix ARRAY OF CHAR from a target ARRAY OF CHAR


