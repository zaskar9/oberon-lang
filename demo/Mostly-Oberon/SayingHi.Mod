(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
MODULE SayingHi;
  IMPORT In, Out;

  PROCEDURE HelloWorld*;
  BEGIN
    Out.String("Hello World!"); Out.Ln;
  END HelloWorld;

  PROCEDURE AskOurName;
  BEGIN
    Out.String("Excuse me, may I ask your name? ");
  END AskOurName;

  PROCEDURE GetName(VAR ourName : ARRAY OF CHAR);
  BEGIN
    In.Line(ourName);
  END GetName;

  PROCEDURE AssembleGreeting(ourName : ARRAY OF CHAR);
  BEGIN
    Out.String("Hello ");Out.String(ourName);
    Out.String (", very nice to meeting you."); Out.Ln;
  END AssembleGreeting;

  PROCEDURE Greetings*;
    VAR ourName : ARRAY 256 OF CHAR;
  BEGIN
    AskOurName;
    GetName(ourName);
    AssembleGreeting(ourName);
  END Greetings;
END SayingHi.