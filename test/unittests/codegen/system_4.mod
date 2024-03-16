(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE System4;

IMPORT SYSTEM, Out;

TYPE
  Date = RECORD day, month, year: INTEGER END;

PROCEDURE Test;
VAR 
    xadr, yadr : LONGINT;
    x, y : Date;
BEGIN
    x.day := 1 ; x.month := 2 ; x.year := 3;
    y.day := -1 ; y.month := -2 ; y.year := -3;
    xadr := SYSTEM.ADR(x);
    yadr := SYSTEM.ADR(y);
    SYSTEM.COPY(xadr, yadr, SYSTEM.SIZE(Date) DIV 4);
    Out.Int(y.day, 0); Out.Ln;
    Out.Int(y.month, 0); Out.Ln;
    Out.Int(y.year, 0); Out.Ln
END Test;

BEGIN
    Test
END System4.
(*
    CHECK: 1
    CHECK: 2
    CHECK: 3
*)