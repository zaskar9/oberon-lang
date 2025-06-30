(**
  StringOps – Basic string manipulation utilities in Oberon.

  This module provides the following procedures:

  - Length:  Return the length of a string.
  - Insert:  Insert a string into another string at a given position.
  - Append:  Append a string at the end of another string.
  - Delete:  Delete characters from a string.
  - Cap:     Convert lowercase ASCII letters in a string to uppercase.
  - Replace: Replace a portion of a string with another string.

  These procedures are designed for null-terminated `ARRAY OF CHAR` strings,
  and always ensure proper 0X-termination.
*)
MODULE Strings;

  (**
    Return the number of characters in `str` up to and excluding the first 0X.

    Parameters:
      str -- The string for which the length will be returned.

    Returns:
      The length of the `str`.

    Example:
      str := "HelloWorld";
      len := Length(src);  (* len becomes 10 *)
  *)
  PROCEDURE Length*(str: ARRAY OF CHAR): INTEGER;
  VAR i: INTEGER;
  BEGIN
    i := 0;
    WHILE (i < LEN(str)) & (str[i] # 0X) DO INC(i) END;
    RETURN i
  END Length;

  (**
    Insert the string `src` into `dst` at position `pos`.
    If `pos = Length(dst)`, `src` is appended to the end of `dst`.
    If the total length exceeds the available space in `dst`, the result is truncated,
    and the string is always null-terminated (with 0X).

    Parameters:
      src -- The string to insert.
      pos -- The position in `dst` at which to insert `src`. Must be in [0, Length(dst)].
      dst -- The destination string (modified in place).

    Example:
      dst := "Hello";
      Insert("World", 5, dst);  (* dst becomes "HelloWorld" *)
  *)
  PROCEDURE Insert*(src: ARRAY OF CHAR; pos: INTEGER; VAR dst: ARRAY OF CHAR);
  VAR srcLen, dstLen, totalLen, dstSize, i: INTEGER;
  BEGIN
    srcLen := Length(src); dstLen := Length(dst);
    (* Make sure that `pos` is within the valid range `[0, dstLen]`. *)
    IF pos < 0 THEN pos := 0 ELSIF pos > dstLen THEN pos := dstLen END;
    (* Calculate available space in `dst`, excluding terminating `0X`. *)
    dstSize := SHORT(LEN(dst) - 1);
    (* Calculate the total length of the resulting string. *)
    totalLen := dstLen + srcLen;
    (* If needed, truncate `srcLen`. *)
    IF totalLen > dstSize THEN
      srcLen := dstSize - dstLen;
      IF srcLen < 0 THEN srcLen := 0 END
    END;
    (* Shift contents of `dst` to the right to make space for contents of `src`. *)
    i := dstLen - 1;
    WHILE (i >= pos) & (i + srcLen < dstSize) DO
      dst[i + srcLen] := dst[i];
      DEC(i)
    END;
    (* Copy `src` into `dst` at position `pos`. *)
    i := 0;
    WHILE (i < srcLen) & (i + pos < dstSize) DO
      dst[i + pos] := src[i];
      INC(i)
    END;
    (* Null-terminate the result. *)
    IF totalLen < LEN(dst) THEN dst[totalLen] := 0X ELSE dst[LEN(dst) - 1] := 0X END
  END Insert;

  (**
    Append a string at the end of another string.
    `Append(str, dst)` has the same effect as `Insert(str, Length(dst), dst)`.
    If the total length exceeds the available space in `dst`, the result is truncated,
    and the string is always null-terminated (with 0X).

    Parameters:
      str -- The string to append.
      dst -- The destination string (modified in place).

    Example:
      dst := "Hello";
      Append("World", dst);  (* dst becomes "HelloWorld" *)
  *)
  PROCEDURE Append*(str: ARRAY OF CHAR; VAR dst: ARRAY OF CHAR);
  BEGIN
    Insert(str, Length(dst), dst)
  END Append;

  (**
    Delete `n` characters from string `str`, starting at position `pos`.
    If `n > Length(str) - pos`, the string is truncated at position `pos`.
    The result is always null-terminated.

    Parameters:
      str -- The input string (modified in place).
      pos -- The position to start deletion (must be in [0, Length(str)]).
      n   -- The number of characters to delete.

    Example:
      str := "abcdef";
      Delete(str, 2, 2);  (* str becomes "abef" *)
  *)
  PROCEDURE Delete*(VAR str: ARRAY OF CHAR; pos, n: INTEGER);
  VAR len, i: INTEGER;
  BEGIN
    len := Length(str);
    (* Check for invalid `pos`. *)
    IF (pos < 0) OR (pos > len) THEN RETURN END;
    (* Truncate the string at `pos`. *)
    IF n >= len - pos THEN
      IF pos < LEN(str) THEN str[pos] := 0X ELSE str[LEN(str) - 1] := 0X END
    ELSE
    (* Shift the remaining characters left. *)
    i := pos;
    WHILE (i + n < len) & (i < LEN(str) - 1) DO
      str[i] := str[i + n];
      INC(i)
    END;
    (* Null-terminate after the new end *)
    IF i < LEN(str) THEN str[i] := 0X ELSE str[LEN(str) - 1] := 0X END
    END
  END Delete;

  (**
    Replace the contents of `dst` starting at position `pos` with `src`.
    This is logically equivalent to `Delete(dst, pos, Length(src))` followed by
    `Insert(src, pos, dst)`, but optimized for in-place performance.

    If the replacement exceeds available space in `dst`, the result is truncated
    to fit, and the final string is always null-terminated.

    Parameters:
      src -- The string to copy into `dst`.
      pos -- The position at which to begin replacing in `dst`.
      dst -- The destination string (modified in place).

    Example:
      dst := "HelloWorld";
      Replace("42", 5, dst);  (* dst becomes "Hello42ld" *)
  *)
  PROCEDURE Replace*(src: ARRAY OF CHAR; pos: INTEGER; VAR dst: ARRAY OF CHAR);
  VAR srcLen, dstLen, i, newLen: INTEGER;
  BEGIN
    srcLen := Length(src); dstLen := Length(dst);
    (* Make sure that `pos` is within the valid range `[0, dstLen]`. *)
    IF pos < 0 THEN pos := 0 ELSIF pos > dstLen THEN pos := dstLen END;
    i := 0;
    WHILE (i < srcLen) & (pos + i < LEN(dst)) DO
      dst[pos + i] := src[i];
      INC(i)
    END;
    (* If the string length increased, adjust null-termination. *)
    newLen := pos + srcLen;
    IF newLen > dstLen THEN
      IF newLen < LEN(dst) THEN dst[newLen] := 0X ELSE dst[LEN(dst) - 1] := 0X END
    END
  END Replace;

  (**
    Extracts (copy) a substring `dst` with `n` characters from position `pos` (`0 <= pos <= Length(src)`) in `src`.
    If `n > Length(src) - pos`, `dst` is only the part of `src` from `pos` to the end of `src`, i.e.
    `Length(src) - 1`. If the size of `dst` is not large enough to hold the result of the operation, the result
    is truncated so that `dst` is always terminated by 0X.

    Parameters:
      src -- The string to copy from.
      pos -- The position to start copying from (must be in [0, Length(str)]).
      n   -- The number of characters to copy.
      dst -- The resulting substring (modified in place).

    Example:
      Extract("HelloWorld", 0, 5, dst);  (* dst becomes "Hello" *)
  *)
  PROCEDURE Extract*(src: ARRAY OF CHAR; pos, n: INTEGER; VAR dst: ARRAY OF CHAR);
  VAR srcLen, i: INTEGER;
  BEGIN
    srcLen := Length(src);
    (* Make sure that `pos` is within the valid range `[0, srcLen]`. *)
    IF pos < 0 THEN pos := 0 ELSIF pos > srcLen THEN pos := srcLen END;
    (* Make sure that `n` is within the valid range `[0, LEN(dst) - 1]. *)
    IF n < 0 THEN n := 0 ELSIF n > LEN(dst) - 1 THEN n := SHORT(LEN(dst) - 1) END;
    i := 0;
    WHILE (i < n) & (pos + i < LEN(src) - 1) DO
      dst[i] := src[pos + i];
      INC(i)
    END;
    dst[i] := 0X
  END Extract;

  (**
    Returns the position of the first occurrence of the substring `sub` in `str`.
    Searching starts at position `pos`. If `sub` is not found, `-1` is returned.

    Parameters:
      sub -- The substring to search for,
      str -- The string to search.
      pos -- The position to start searching from (must be in [0, Length(str)]).

    Returns:
      The position of the first occurrence of the substring or `-1` if the substring is not found.

    Example:
      pos := Pos("World", "HelloWorld", 0);  (* pos becomes 5 *)
  *)
  PROCEDURE Pos*(sub, str: ARRAY OF CHAR; pos: INTEGER): INTEGER;
  VAR srcLen, subLen, i, j, res: INTEGER;
  BEGIN
    srcLen := Length(str); subLen := Length(sub); res := -1;
    (* Make sure that `pos` is within the valid range `[0, srcLen]`. *)
    IF pos < 0 THEN pos := 0 ELSIF pos > srcLen THEN pos := srcLen END;
    (* Make sure that the length of the substring does not exceed the length of the string. *)
    IF (subLen <= srcLen) & (pos + subLen <= srcLen) THEN
      i := pos;
      j := 0;
      WHILE (res < 0) & (sub[j] # 0X) & (str[i + j] # 0X) DO
        WHILE (sub[j] # 0X) & (str[i + j] = sub[j]) DO
          INC(j)
        END;
        IF sub[j] = 0X THEN
          res := i (* Found a match! *)
        ELSE
          INC(i);
          j := 0
        END
      END
    END;
    RETURN res
  END Pos;

  (**
    Convert all lowercase ASCII letters in `str` (a-z) to uppercase (A-Z).
    Other characters remain unchanged.

    Parameters:
      str -- The input string (modified in place).

    Example:
      str := "Hello World!";
      Cap(str);  (* str becomes "HELLO WORLD!" *)
  *)
  PROCEDURE Cap*(VAR str: ARRAY OF CHAR);
  VAR i: INTEGER;
  BEGIN
    i := 0;
    WHILE (i < LEN(str)) & (str[i] # 0X) DO
      IF ("a" <= str[i]) & (str[i] <= "z") THEN
        str[i] := CHR(ORD(str[i]) - ORD("a") + ORD("A"))
      END;
      INC(i)
    END
  END Cap;

END Strings.