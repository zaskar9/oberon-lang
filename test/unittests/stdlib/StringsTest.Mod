(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE StringsTest;
IMPORT Out, Strings;

  PROCEDURE PrintResult(testName: ARRAY OF CHAR; success: BOOLEAN);
  BEGIN
    IF success THEN
      Out.String(testName); Out.String(": PASS"); Out.Ln
    ELSE
      Out.String(testName); Out.String(": FAIL"); Out.Ln
    END;
    ASSERT(success)
  END PrintResult;

  PROCEDURE TestLength;
  VAR s: ARRAY 16 OF CHAR;
  BEGIN
    s := "0123456789";
    PrintResult("TestLengthVariable", Strings.Length(s) = 10);
    s := "Hello World!";
    PrintResult("TestLengthUpdateNonEmpty", Strings.Length(s) = 12);
    s := "";
    PrintResult("TestLengthUpdateEmpty", Strings.Length(s) = 0);
    PrintResult("TestLengthEmptyLiteral", Strings.Length("") = 0);
    PrintResult("TestLengthOneLiteral", Strings.Length(" ") = 1)
  END TestLength;

  PROCEDURE TestInsertAtStart;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "World";
    Strings.Insert("Hello", 0, s);
    PrintResult("TestInsertAtStart", s = "HelloWorld")
  END TestInsertAtStart;

  PROCEDURE TestInsertAtMiddle;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "HellWorld";
    Strings.Insert("o", 4, s);
    PrintResult("TestInsertAtMiddle", s = "HelloWorld")
  END TestInsertAtMiddle;

  PROCEDURE TestInsertAtEnd;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "Hello";
    Strings.Insert("World", 5, s);
    PrintResult("TestInsertAtEnd", s = "HelloWorld")
  END TestInsertAtEnd;

  PROCEDURE TestInsertTruncated;
    VAR s: ARRAY 10 OF CHAR;
  BEGIN
    s := "Hello";
    Strings.Insert("World", 5, s);
    PrintResult("TestInsertTruncated", s = "HelloWorl")
  END TestInsertTruncated;

  PROCEDURE TestInsertEmptySrc;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "Hello";
    Strings.Insert("", 2, s);
    PrintResult("TestInsertEmptySrc", s = "Hello")
  END TestInsertEmptySrc;

  PROCEDURE TestInsertEmptyDst;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "";
    Strings.Insert("Hello", 0, s);
    PrintResult("TestInsertEmptyDst", s = "Hello")
  END TestInsertEmptyDst;
  
  PROCEDURE TestDeleteFromStart;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s :=  "HelloWorld";
    Strings.Delete(s, 0, 5);
    PrintResult("TestDeleteFromStart", s = "World")
  END TestDeleteFromStart;
  
  PROCEDURE TestDeleteFromMiddle;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s :=  "HelloWorld";
    Strings.Delete(s, 3, 2);
    PrintResult("TestDeleteFromMiddle", s = "HelWorld")
  END TestDeleteFromMiddle;
  
  PROCEDURE TestDeleteFromEnd;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s :=  "HelloWorld";
    (* Deleting past the end. *)
    Strings.Delete(s, 5, 10);
    PrintResult("TestDeleteFromEnd", s = "Hello")
  END TestDeleteFromEnd;
  
  PROCEDURE TestDeleteZeroChars;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s :=  "Hello";
    Strings.Delete(s, 2, 0);
    PrintResult("TestDeleteZeroChars", s = "Hello")
  END TestDeleteZeroChars;
  
  PROCEDURE TestDeleteInvalidPos;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s :=  "Hello";
    (* Should do nothing as `pos > Length(str)`. *)
    Strings.Delete(s, 10, 3);
    PrintResult("TestDeleteInvalidPos", s = "Hello")
  END TestDeleteInvalidPos;
  
  PROCEDURE TestDeleteWholeString;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s :=  "Hello";
    Strings.Delete(s, 0, 100);
    PrintResult("TestDeleteWholeString", s = "")
  END TestDeleteWholeString;
  
  PROCEDURE TestCapSimple;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "hello";
    Strings.Cap(s);
    PrintResult("TestCapSimple", s = "HELLO")
  END TestCapSimple;
  
  PROCEDURE TestCapMixed;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "Hello World!";
    Strings.Cap(s);
    PrintResult("TestCapMixed", s = "HELLO WORLD!")
  END TestCapMixed;
  
  PROCEDURE TestCapAllCaps;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "UPPERCASE";
    Strings.Cap(s);
    PrintResult("TestCapAllCaps", s = "UPPERCASE")
  END TestCapAllCaps;
  
  PROCEDURE TestCapDigitsAndSymbols;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "1234!@#abcXYZ";
    Strings.Cap(s);
    PrintResult("TestCapDigitsAndSymbols", s = "1234!@#ABCXYZ")
  END TestCapDigitsAndSymbols;
  
  PROCEDURE TestCapEmptyString;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "";
    Strings.Cap(s);
    PrintResult("TestCapEmptyString", s = "")
  END TestCapEmptyString;

  PROCEDURE TestReplaceAtStart;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "HelloWorld";
    Strings.Replace("Ha", 0, s);
    PrintResult("TestReplaceAtStart", s = "HalloWorld")
  END TestReplaceAtStart;

  PROCEDURE TestReplaceAtMiddle;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "HelloWorld";
    Strings.Replace("42", 2, s);
    PrintResult("TestReplaceAtMiddle", s = "He42oWorld")
  END TestReplaceAtMiddle;

  PROCEDURE TestReplaceAtEnd;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "Hello";
    Strings.Replace("XYZ", 4, s);
    PrintResult("TestReplaceAtEnd", s = "HellXYZ")
  END TestReplaceAtEnd;

  PROCEDURE TestReplaceShorterThanOriginal;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "abcdef";
    (* Replaces "cd" with "XY". *)
    Strings.Replace("XY", 2, s);
    PrintResult("TestReplaceShorterThanOriginal", s = "abXYef")
  END TestReplaceShorterThanOriginal;

  PROCEDURE TestReplaceLongerThanOriginal;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "abcdef";
    Strings.Replace("12345", 2, s);
    PrintResult("TestReplaceLongerThanOriginal", s = "ab12345")
  END TestReplaceLongerThanOriginal;

  PROCEDURE TestReplaceTruncating;
    VAR s: ARRAY 10 OF CHAR;
  BEGIN
    s := "123456789";
    (* Should truncate "ABCDEFGH" to fit buffer, *)
    Strings.Replace("ABCDEFGH", 5, s);
    PrintResult("TestReplaceTruncating", s = "12345ABCD")
  END TestReplaceTruncating;

  PROCEDURE TestReplaceEmptySrc;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "HelloWorld";
    (* Deletes 0 chars and shifts. *)
    Strings.Replace("", 3, s);
    PrintResult("TestReplaceEmptySrc", s = "HelloWorld")
  END TestReplaceEmptySrc;

  PROCEDURE TestReplaceIntoEmptyDst;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    s := "";
    Strings.Replace("Insert", 0, s);
    PrintResult("TestReplaceIntoEmptyDst", s = "Insert")
  END TestReplaceIntoEmptyDst;

  PROCEDURE TestExtractAtStart;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    Strings.Extract("HelloWorld", 0, 5, s);
    PrintResult("TestExtractAtStart", s = "Hello")
  END TestExtractAtStart;

  PROCEDURE TestExtractEmpty;
    VAR s: ARRAY 32 OF CHAR;
  BEGIN
    Strings.Extract("HelloWorld", 0, -1, s);
    PrintResult("TestExtractEmpty", s = "")
  END TestExtractEmpty;

  PROCEDURE TestExtractTruncated;
    VAR s: ARRAY 8 OF CHAR;
  BEGIN
    Strings.Extract("HelloWorld", 0, 10, s);
    PrintResult("TestExtractTruncated", s = "HelloWo")
  END TestExtractTruncated;

  PROCEDURE TestPosAtStart;
    VAR pos: INTEGER;
  BEGIN
    pos := Strings.Pos("Hello", "HelloWorld", 0);
    PrintResult("TestPosAtStart", pos = 0)
  END TestPosAtStart;

  PROCEDURE TestPosAtMiddle;
    VAR pos: INTEGER;
  BEGIN
    pos := Strings.Pos("Hello", "HelloWorld", 5);
    PrintResult("TestPosAtMiddle", pos = -1)
  END TestPosAtMiddle;

  PROCEDURE TestPosAtEnd;
    VAR pos: INTEGER;
  BEGIN
    pos := Strings.Pos("Hello", "HelloWorld", 10);
    PrintResult("TestPosAtEnd", pos = -1)
  END TestPosAtEnd;

  PROCEDURE TestPosTwice;
    VAR pos: INTEGER;
  BEGIN
    pos := Strings.Pos("abc", "abcabc", 0);
    INC(pos);
    pos := Strings.Pos("abc", "abcabc", pos);
    PrintResult("TestPosTwice", pos = 3)
  END TestPosTwice;

  PROCEDURE TestPosInvalid;
    VAR pos: INTEGER;
  BEGIN
    pos := Strings.Pos("HelloWorld", "Hello", 0);
    PrintResult("TestPosInvalid", pos = -1)
  END TestPosInvalid;

BEGIN
  (* Strings.Length tests *)
  TestLength;

  (* Strings.Insert tests *)
  TestInsertAtStart;
  TestInsertAtMiddle;
  TestInsertAtEnd;
  TestInsertTruncated;
  TestInsertEmptySrc;
  TestInsertEmptyDst;

  (* Strings.Delete tests *)
  TestDeleteFromStart;
  TestDeleteFromMiddle;
  TestDeleteFromEnd;
  TestDeleteZeroChars;
  TestDeleteInvalidPos;
  TestDeleteWholeString;

  (* Strings.Cap tests *)
  TestCapSimple;
  TestCapMixed;
  TestCapAllCaps;
  TestCapDigitsAndSymbols;
  TestCapEmptyString;

  (* Strings.Replace tests *)
  TestReplaceAtStart;
  TestReplaceAtMiddle;
  TestReplaceAtEnd;
  TestReplaceShorterThanOriginal;
  TestReplaceLongerThanOriginal;
  TestReplaceTruncating;
  TestReplaceEmptySrc;
  TestReplaceIntoEmptyDst;

  (* Strings.Extract tests *)
  TestExtractAtStart;
  TestExtractEmpty;
  TestExtractTruncated;

  (* Strings.Pos tests *)
  TestPosAtStart;
  TestPosAtMiddle;
  TestPosAtEnd;
  TestPosTwice;
  TestPosInvalid
END StringsTest.