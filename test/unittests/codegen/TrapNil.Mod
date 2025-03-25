(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
  XFAIL: *
*)
MODULE TrapNil;
IMPORT Out;

TYPE XYPoint = POINTER TO XYPointDesc;
     XYPointDesc = RECORD x, y: INTEGER END;

VAR p: XYPoint;

PROCEDURE Print(p: XYPoint);
BEGIN
    Out.Char("["); Out.Int(p.x, 0); Out.Char(","); Out.Int(p.y, 0); Out.Char("]")
END Print;

BEGIN
    p := NIL;
    Print(p); Out.Ln
END TrapNil.
