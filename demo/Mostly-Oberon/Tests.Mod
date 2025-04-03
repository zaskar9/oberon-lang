(** Tests.Mod - A module providing simple test support for Oberon-7.

Copyright (C) 2021 R. S. Doiel

Released under The 3-Clause BSD License.
See https://opensource.org/licenses/BSD-3-Clause

*)
MODULE Tests; (** portable *)
IMPORT Out;

CONST
  MAXSET = 31; (* Oberon-7 doesn't have MAX(SET), use 0 to 31 *)
  MAXSTR* = 1024;  (* Maximum string length used in tests *)

TYPE
    (** TestProc is the signature of a test procedure. It is simple.
        if a test successeds it returns true, otherwise it returns false.
        The procedure Test counts the number of test run and results
        updating variable parameters of success and errors. In turn
        these are passed to Summarize to return a summary report. *)
    TestProc* = PROCEDURE () : BOOLEAN;

    TestSet* = POINTER TO TestSetDesc;
    TestSetDesc* = RECORD
                    title* : ARRAY MAXSTR OF CHAR;
                    fn* : TestProc;
                    next* : TestSet
                  END;


PROCEDURE Length(source : ARRAY OF CHAR) : INTEGER;
  VAR i, l : INTEGER;
BEGIN i := 0; l := LEN(source);
  WHILE (i < l) & (source[i] # 0X) DO INC(i); END;
  RETURN i 
END Length;

(** DisplayString display quoted ARRAY OF CHAR with prefixed by msg *)
PROCEDURE DisplayString*(msg: ARRAY OF CHAR; source : ARRAY OF CHAR);
BEGIN
  Out.String(msg);Out.String(" '"); Out.String(source); Out.String("'");
  Out.Ln();
END DisplayString;

(* displayStrings display expected (s1) and calculated (s2) strings
   and if they don't match display error message and strings. *)
PROCEDURE displayStrings(s1, s2, msg: ARRAY OF CHAR);
BEGIN
  Out.String("Expected '");Out.String(s1);
  Out.String("', got '"); Out.String(s2); Out.String("' ");
  Out.String(msg);Out.Ln();
END displayStrings;

(** ExpectedInt compares to int display msg on error and updates test to
   FALSE if they don'y match *)
PROCEDURE ExpectedInt*(expected, got : INTEGER; msg : ARRAY OF CHAR; VAR test : BOOLEAN);
BEGIN
  IF expected # got THEN
     Out.String("Expected ");Out.Int(expected, 1);
     Out.String(", got ");Out.Int(got, 1);
     Out.String(" "); Out.String(msg); Out.Ln();
     test := FALSE;
  END;
END ExpectedInt;

(** ExpectedReal compares to REAL display msg on error and updates test to
   FALSE if they don'y match *)
PROCEDURE ExpectedReal*(expected, got : REAL; msg : ARRAY OF CHAR; VAR test : BOOLEAN);
BEGIN
  IF expected # got THEN
     Out.String("Expected ");Out.Real(expected, 1);
     Out.String(", got ");Out.Real(got, 1);
     Out.String(" "); Out.String(msg); Out.Ln();
     test := FALSE;
  END;
END ExpectedReal;

(** ExpectedString compare two ARRAY OF CHAR, set test to FALSE
    if they don't match and display msg *)
PROCEDURE ExpectedString*(s1, s2, msg : ARRAY OF CHAR; VAR test : BOOLEAN);
  VAR i, lengthS1, lengthS2 : INTEGER; ok : BOOLEAN;
BEGIN ok := TRUE;
  lengthS1 := Length(s1);
  lengthS2 := Length(s2);
  IF lengthS1 # lengthS2 THEN
    displayStrings(s1, s2, msg); test := FALSE;
  ELSE
    i := 0;
    ok := TRUE;
    WHILE ok & (i < lengthS1) DO
      IF s1[i] # s2[i] THEN
        displayStrings(msg, s1, s2); test := FALSE; 
        ok := FALSE;
      END;
      INC(i);
    END;
  END;
END ExpectedString;

(** ExpectedChar compare two CHAR, set test to FALSE if they don't
    match and display msg *)
PROCEDURE ExpectedChar*(expected, got : CHAR; msg : ARRAY OF CHAR; VAR test : BOOLEAN);
BEGIN
  IF expected # got THEN
    test := FALSE;
    Out.String("Expected '");Out.Char(expected); Out.String("', got '");
    Out.Char(got); Out.String("' ");
    Out.String(msg);
    Out.Ln;
  END;
END ExpectedChar;

(** ExpectedBool compare to BOOLEAN values, set test to FALSE if they
    don't match and display message *)
PROCEDURE ExpectedBool*(expected, got : BOOLEAN; msg : ARRAY OF CHAR; VAR test : BOOLEAN);
  PROCEDURE displayBool( val : BOOLEAN);
  BEGIN 
    IF val THEN
      Out.String("TRUE");
    ELSE
      Out.String("FALSE");
    END;
  END displayBool;

BEGIN
  (* NOTE: Odd behavior. I would expect to that the following should NOT display
  output when both "expected" and "got" are TRUE. Compiler is OBNC 0.16.1.

    IF expected # got THEN
      ...

  This also exhibits the same behavior.

    IF (expected # got) THEN
    ...

    IF (expected # got) = TRUE THEN
    ...

  However checking explicitly for value of "expected" and "got" and making explicit
  tests does work.

    IF ((expected = TRUE) & (got = FALSE)) & ((expected = FALSE) & (got = TRUE)) THEN
    ...

  Is this a subtitly in the translation of Oberon-07 to C combined with lang-1000.11.45.5
  on Darin 17.7.0?
 *)
  IF ((expected = TRUE) & (got = FALSE)) OR ((expected = FALSE) & (got = TRUE)) THEN
      Out.String("Expected ");
      displayBool(expected);
      Out.String(", got ");
      displayBool(got);
      Out.String(" "); Out.String(msg); Out.Ln;
      test := FALSE;
  END;
END ExpectedBool;

(** ExpectedBytes compares the first N values to two array of byte *)
PROCEDURE ExpectedBytes*(expected, got : ARRAY OF BYTE; n : INTEGER; msg : ARRAY OF CHAR; VAR test: BOOLEAN);
  VAR i, l : INTEGER;

  PROCEDURE minimum(a, b : INTEGER) : INTEGER;
    VAR res : INTEGER;
  BEGIN 
    IF a < b THEN res := a ELSE res := b END;
    RETURN res
  END minimum;

BEGIN
  l := minimum(LEN(expected), LEN(got));
  IF n <= l THEN
    i := 0;
    WHILE test & (i < (n - 1)) DO
      IF expected[i] # got[i] THEN
        test := FALSE;
        Out.String("Expected (pos: ");Out.Int(i, 1);Out.String(") ");
        Out.Int(expected[i], 1);Out.String(", got ");Out.Int(got[i], 1); 
        Out.String(" ");
        Out.String(msg); Out.Ln();
      END;
      INC(i);
    END;
  ELSE
    test := FALSE;
    Out.String("Expected length N (");Out.Int(n, 1);Out.String("), got length (");Out.Int(l, 1); Out.String(") ");
    Out.String(msg); Out.Ln();
  END;
END ExpectedBytes;

PROCEDURE displaySet(s : SET);
  VAR i : INTEGER;
BEGIN
  Out.Char("{");
  FOR i := 0 TO MAXSET DO
    IF i > 0 THEN
      Out.String(", ");
    END;
    Out.Int(i, 1);
  END;
  Out.Char("}");
END displaySet;

(**ExpectedSet compares two sets, display message if they don't match and
   set the value of test to FALSE *)
PROCEDURE ExpectedSet*(expected, got : SET; msg : ARRAY OF CHAR; VAR test : BOOLEAN);
BEGIN
  IF expected = got THEN
    Out.String("Expected set ");displaySet(expected);
    Out.String(" got ");displaySet(got); Out.String(" ");
    Out.String(msg);Out.Ln();
    test := FALSE;
  END;
END ExpectedSet;

(** ShowTitle displays the title in standard out and underlined with '=' *)
PROCEDURE ShowTitle*(s : ARRAY OF CHAR);
  VAR i, l : INTEGER;
BEGIN l := Length(s) - 1;
  Out.Ln();
  Out.String(s); Out.Ln();
  FOR i := 0 TO l DO
    Out.String("=");
  END;
  Out.Ln();
END ShowTitle;

(** Test -- run the test method and update the success and error variables
provided. *)
PROCEDURE Test*(fn : TestProc; VAR success : INTEGER; VAR errors : INTEGER);
BEGIN
    IF fn() THEN
        success := success + 1;
    ELSE
        errors := errors + 1;
    END
END Test;

(** Summarize -- sumarize the results using the test title, success
and error counts. *)
PROCEDURE Summarize*(title : ARRAY OF CHAR; successes, errors : INTEGER);
BEGIN
  IF errors > 0 THEN
    Out.Ln();
    Out.String("Success: ");Out.Int(successes, 5);Out.Ln();
    Out.String(" Errors: ");Out.Int(errors, 5);Out.Ln();
    Out.String("-------------------------------------------");Out.Ln();
    Out.String("  Total: ");Out.Int(successes + errors, 5);Out.Ln();
    Out.Ln();
    Out.String(title);Out.String(" failed.");Out.Ln();
    ASSERT(FALSE);
  ELSE
    Out.String("OK, ");Out.String(title); Out.Ln();
  END;
END Summarize;

(** New initializes a new TestSet with a title *)
PROCEDURE Init*(VAR ts: TestSet; title : ARRAY OF CHAR);
BEGIN
  IF ts = NIL THEN
    NEW(ts);
  END;
  ts.title := title;
  ts.fn := NIL;
  ts.next := NIL;
END Init;

PROCEDURE Add*(VAR ts : TestSet; fn : TestProc);
  VAR cur : TestSet;
BEGIN
   cur := ts;
   IF cur.fn # NIL THEN
     (* Move to end of list *)
     WHILE cur.next # NIL DO cur := cur.next END;
     NEW(cur.next);
     cur := cur.next;
   END;
   cur.fn := fn;
   cur.next := NIL;
END Add;

PROCEDURE Run*(ts : TestSet) : BOOLEAN;
  VAR title : ARRAY MAXSTR OF CHAR;
      i, successes, errors : INTEGER;
      ok, result : BOOLEAN;
BEGIN 
  successes := 0; errors := 0;
  FOR i := 0 TO LEN(ts.title) - 1 DO
    title[i] := ts.title[i];
  END;
  ok := TRUE; 
  WHILE ts # NIL DO
    IF ts.fn # NIL THEN
      result := ts.fn();
      IF result THEN
        INC(successes);
      ELSE
        INC(errors);
        ok := FALSE;
      END;
    END;
    ts := ts.next;
  END;
  IF ok THEN
    Out.String("OK, "); Out.String(title); Out.Ln();
  ELSE
    ShowTitle(title);
    Summarize(title, successes, errors);
  END;
  RETURN ok
END Run;

END Tests.
{
    "title": "Tests",
    "description": "An Oberon-7 simple test module."
    "byline: "R. S. Doiel, 2020-11-09",
    "pubDate": "2020-11-09",
    "keywords": [ "oberon", "testing" ],
    "license": "http://www.gnu.org/licenses/agpl-3.0.html"
}

Tests
=====

This module provide a few simple method for implementing module
testing of exported methods and variables.

Test
----

This procedure provides a simple way to run a test method
and track the success or failure of the test. Test proceedures
hare parameterless returning a TRUE on success and FALSE if
an error in encountered.

Summarize
---------

This procedure takes a title, success and error counts and returns
the results. If error count is greater than zero then program
exists with an error by calling `ASSERT(FALSE);`.



