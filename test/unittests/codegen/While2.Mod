(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE While2;
IMPORT Out;

VAR m, n: INTEGER;

PROCEDURE Test1(i: INTEGER);
BEGIN
    WHILE i < 10 DO
       i := i + 1;
       Out.Int(i, 1); Out.String(" ")
    ELSIF i < 100 DO
       i := i + 10;
       Out.Int(i, 1); Out.String(" ")
    ELSIF i < 1000 DO
       i := i + 100;
       Out.Int(i, 1); Out.String(" ")
    END
END Test1;

PROCEDURE Test2(VAR m, n: INTEGER);
BEGIN
    WHILE m > n DO m := m - n
    ELSIF n > m DO n := n - m
    END
END Test2;

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

BEGIN
    Test1(1000);
    m := 100; n := 1000;
    IF m > n THEN m := m - n ELSIF n > m THEN n := n - m END;
    Test2(m, n);
    Out.Int(m, 1); Out.String(" "); Out.Int(n, 1); Out.Ln
END While2.