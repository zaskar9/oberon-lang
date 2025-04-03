(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
MODULE LoopsAndConditions;
  IMPORT In, Out;

PROCEDURE IfElsifElseDemo;
  VAR s : ARRAY 128 OF CHAR;
BEGIN
  Out.String("Enter your name: ");
  In.Line(s);
  Out.Ln;
  IF (s = "Freda") OR (s = "Mojo") THEN
    Out.String("Wowie Zowie! I remember you from ZBS stories."); Out.Ln;
  ELSIF (s = "Bilbo") OR (s = "Gandalf") THEN
    Out.String("Greets, I remember from the Hobbit."); Out.Ln;
  ELSE
    Out.String("Glad to meet you "); Out.String(s); Out.Ln;
  END;
END IfElsifElseDemo;

PROCEDURE AssertDemo;
  VAR s : ARRAY 128 OF CHAR;
BEGIN
  Out.String("Should I continue? Y/n ");
  In.Line(s);
  Out.Ln;
  ASSERT((s = "Y") OR (s = "y"));
END AssertDemo;

PROCEDURE WhileDemo;
  VAR i : INTEGER;
BEGIN
  Out.String("Basic WHILE counting from 1 to 10");Out.Ln;
  i := 0;
  WHILE i < 10 DO
     i := i + 1;
     Out.Int(i, 1);Out.String(" ");
  END;
  Out.Ln;
  Out.String("WHILE ELSIF, count 1 to 10 THEN 10 to 100");Out.Ln;
  i := 0;
  WHILE i < 10 DO
     i := i + 1;
     Out.Int(i, 1); Out.String(" ");
  ELSIF i < 100 DO
     i := i + 10;
     Out.Int(i, 1);Out.String(" ");
  END;
  Out.Ln;
  Out.String("Demo of while loop counting one to ten, then by tenths.");
END WhileDemo;

PROCEDURE ForDemo;
  VAR i : INTEGER;
BEGIN
  Out.String("Basic FOR LOOP counting from 1 to 10");Out.Ln;
  FOR i := 1 TO 10 DO
     Out.Int(i, 1);Out.String(" ");
  END;
  Out.Ln;
  Out.String("FOR loop counting by twos 1 to 20");Out.Ln;
  FOR i := 0 TO 20 BY 2  DO
     Out.Int(i, 1);Out.String(" ");
  END;
  Out.Ln;
END ForDemo;

BEGIN
  IfElsifElseDemo;
  AssertDemo;
  WhileDemo;
  ForDemo;
END LoopsAndConditions.