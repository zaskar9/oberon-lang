(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Fails with Segmentation fault
*)
MODULE Array5;
IMPORT Out;

TYPE ProcType1 = PROCEDURE;
     ProcType2 = PROCEDURE(): INTEGER;
     ProcType3 = PROCEDURE(x: INTEGER);
     ProcType4 = PROCEDURE(x: INTEGER): INTEGER;
     ProcType5 = PROCEDURE: INTEGER;
     ProcType6 = PROCEDURE(x, y: INTEGER; z: INTEGER): REAL;
     ProcType7 = PROCEDURE(x, y: INTEGER): INTEGER;

VAR p: ProcType7;

PROCEDURE P;
BEGIN Out.Int(1, 0); Out.Ln END P;

PROCEDURE Sum(x, y: INTEGER): INTEGER;
BEGIN
    RETURN x + y
END Sum;

PROCEDURE Test;
VAR
  a: ARRAY 3 OF RECORD p: PROCEDURE; f: ProcType7 END;
  i: INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i].p := P;
    a[i].f := Sum
  END;
  FOR i := 0 TO 2 DO
    a[i].p;
    Out.Int(a[i].f(i, i), 0); Out.Ln
  END
END Test;

BEGIN
    Test;
    p := Sum;
    Out.Int(p(1, 2), 0); Out.Ln
END Array5.
(*
    CHECK: 1
    CHECK: 1
    CHECK: 1
*)