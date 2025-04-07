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

PROCEDURE Proc;
BEGIN
  (* Out.Int(1, 0); Out.Ln *)
END Proc;

PROCEDURE One(): INTEGER;
BEGIN
  RETURN 1
END One;

PROCEDURE Sum(x, y: INTEGER): INTEGER;
BEGIN
    RETURN x + y
END Sum;

PROCEDURE Test;
VAR
  a: ARRAY 3 OF RECORD proc: PROCEDURE; foo: ProcType7; bar: ProcType5 END;
  i: INTEGER;
BEGIN
  FOR i := 0 TO 2 DO
    a[i].proc := Proc;
    (* a[i].foo := Sum;
    a[i].bar := One *)
  END;
  FOR i := 0 TO 2 DO
    a[i].proc;
    Out.Int(a[i].foo(i, i), 0); Out.Ln;
    Out.Int(a[i].bar(), 0); Out.Ln
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