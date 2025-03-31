(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
MODULE BasicTypeDemo;
  IMPORT Out;

  (* These are our custom data types definitions. *)
  TYPE
      TopThreeScoreboard = RECORD
        gameName : ARRAY 24 OF CHAR;
        playerNames : ARRAY 3, 24 OF CHAR;
        scores : ARRAY 3 OF INTEGER
      END;

      DStringDesc = RECORD
        value : CHAR;
        next : POINTER TO DStringDesc
      END;

      DString = POINTER TO DStringDesc;

  (* Here are our private variables. *)
  VAR
    i : INTEGER;
    a : REAL;
    c: CHAR;
    name : ARRAY 24 OF CHAR;
    scores : ARRAY 10 OF INTEGER;
    scoreboard : TopThreeScoreboard;
    s : DString;


  PROCEDURE SimpleTypes;
  BEGIN
    i := 7;
    a := 7.1;
    c := "Z"
  END SimpleTypes;

  PROCEDURE DisplaySimpleTypes;
  BEGIN
    Out.String(" i: ");Out.Int(i, 1); Out.Ln;
    Out.String(" a: ");Out.Real(a, 1); Out.Ln;
    Out.String(" c: ");Out.Char(c); Out.Ln
  END DisplaySimpleTypes;


  PROCEDURE MoreComplexTypes;
  BEGIN
    scores[0] := 102;
    name := "Ada Lovelace";
    scoreboard.gameName := "Basketball";
    scoreboard.playerNames[0] := "Ada Lovelace";
    scoreboard.scores[0] := 102;
    scoreboard.playerNames[1] := "Blaise Pascal";
    scoreboard.scores[1] := 101;
    scoreboard.playerNames[2] := "John McCarthy";
    scoreboard.scores[2] := 100
  END MoreComplexTypes;

  PROCEDURE DisplayMoreComplexTypes;
    VAR i : INTEGER;
  BEGIN
    i := 0;
    Out.String(" Game: "); Out.String(scoreboard.gameName); Out.Ln;
    WHILE i < LEN(scoreboard.playerNames) DO
      Out.String("    player, score: ");
      Out.String(scoreboard.playerNames[i]);Out.String(", ");
      Out.Int(scoreboard.scores[i], 1);
      Out.Ln;
      i := i + 1
    END
  END DisplayMoreComplexTypes;

  PROCEDURE SetDString(VAR s : DString; buf : ARRAY OF CHAR);
      VAR i : INTEGER; cur, tmp : DString;
  BEGIN
    (* Handle the case where s is NIL *)
    IF s = NIL THEN
      NEW(s);
      s.value := 0X;
      s.next := NIL
    END;
    cur := s;
    i := 0;
    (* check to see if we are at end of string or array *)
    WHILE (buf[i] # 0X) & (i < LEN(buf)) DO
      cur.value := buf[i];
      IF cur.next = NIL THEN
        NEW(tmp);
        tmp.value := 0X;
        tmp.next := NIL;
        cur.next := tmp
      END;
      cur := cur.next;
      i := i + 1
    END
  END SetDString;

  PROCEDURE DStringToCharArray(s : DString; VAR buf : ARRAY OF CHAR);
    VAR cur : DString; i, l : INTEGER;
  BEGIN
    l := SHORT(LEN(buf));   (* TODO: can this `SHORT` be avoided by full O07 compliance? *)
    i := 0;
    cur := s;
    WHILE (i < l) & (cur # NIL) DO
      buf[i] := cur.value;
      cur := cur.next;
      i := i + 1
    END;
    (* Zero out the rest of the string. *)
    WHILE (i < l) DO
      buf[i] := 0X;
      i := i + 1
    END
  END DStringToCharArray;

BEGIN
  SimpleTypes;
  DisplaySimpleTypes;
  MoreComplexTypes;
  DisplayMoreComplexTypes;
  (* Demo our dynamic string *)
  Out.String("Copy the phrase 'Hello World!' into our dynamic string"); Out.Ln;
  SetDString(s, "Hello World!");
  Out.String("Copy the value of String s into 'name' our array of char"); Out.Ln;
  DStringToCharArray(s, name);
  Out.String("Display 'name' our array of char: "); Out.String(name); Out.Ln
END BasicTypeDemo.