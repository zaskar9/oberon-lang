(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE While2;
IMPORT Out;

VAR m, n: INTEGER;

PROCEDURE Test1a();
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE i < 10 DO
       i := i + 1;
       Out.Int(i, 1); Out.String(" ")
    ELSIF i < 100 DO
       i := i + 10;
       Out.Int(i, 1); Out.String(" ")
    ELSIF i < 1000 DO
       i := i + 100;
       Out.Int(i, 1); Out.String(" ")
    END;
    Out.String("Done!")
END Test1a;

PROCEDURE Test1b();
VAR i: INTEGER;
BEGIN
    i := 0;
    LOOP
        IF i < 10 THEN
           i := i + 1;
           Out.Int(i, 1); Out.String(" ")
        ELSIF i < 100 THEN
           i := i + 10;
           Out.Int(i, 1); Out.String(" ")
        ELSIF i < 1000 THEN
           i := i + 100;
           Out.Int(i, 1); Out.String(" ")
        ELSE
           EXIT
        END
    END;
    Out.String("Done!")
END Test1b;

PROCEDURE Test1c();
VAR i: INTEGER;
BEGIN
    i := 0;
    LOOP
        CASE i OF
              0..9: i := i + 1;   Out.Int(i, 1); Out.String(" ")
        |   10..99: i := i + 10;  Out.Int(i, 1); Out.String(" ")
        | 100..999: i := i + 100; Out.Int(i, 1); Out.String(" ")
        ELSE
           EXIT
        END
    END;
    Out.String("Done!")
END Test1c;

PROCEDURE Test2a(VAR m, n: INTEGER);
BEGIN
    WHILE m > n DO m := m - n
    ELSIF n > m DO n := n - m
    END
END Test2a;

PROCEDURE Test2b(VAR m, n: INTEGER);
BEGIN
    LOOP
        IF m > n THEN m := m - n ELSIF n > m THEN n := n - m ELSE EXIT END
    END
END Test2b;

PROCEDURE Test3(VAR x: INTEGER);
VAR i: INTEGER;
BEGIN
    i := 0;
    IF x < i THEN i := x; x := 0 END;
    LOOP
        IF i >= x THEN EXIT END;
        Out.Int(i, 1); Out.Char(" ");
        INC(i)
    END
END Test3;

PROCEDURE Test4(x, y: INTEGER): INTEGER;
BEGIN
    IF x > y THEN RETURN x ELSE RETURN y END
END Test4;

PROCEDURE Test5();
VAR i: INTEGER;
BEGIN
    i := 0;
    REPEAT
        INC(i);
        IF i = 0 THEN RETURN ELSE EXIT END
    UNTIL i > 10;
    ASSERT(i = 1)
END Test5;

BEGIN
    Test1a(); Out.Ln;
    Test1b(); Out.Ln;
    Test1c(); Out.Ln;
    m := 27; n := 9;
    Test2a(m, n);
    ASSERT((m = 9) & (n = 9));
    m := 27; n := 9;
    Test2b(m, n);
    ASSERT((m = 9) & (n = 9));
    Test5
END While2.
(*
    CHECK: 1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900 1000 Done!
    CHECK: 1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900 1000 Done!
    CHECK: 1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900 1000 Done!
*)