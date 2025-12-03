(*
  RUN: %oberon --std=Oberon2 --run %s
*)
MODULE DynamicArray1;
IMPORT Out;

  TYPE
    Buffer = POINTER TO ARRAY OF CHAR;
    Procedure = PROCEDURE(a: ARRAY OF CHAR);

  VAR
    buf1: Buffer;
    buf2: POINTER TO ARRAY OF CHAR;
    buf3: POINTER TO ARRAY OF ARRAY OF CHAR;
    buf4: POINTER TO ARRAY OF ARRAY 8 OF CHAR;

  PROCEDURE proc1(a: ARRAY OF CHAR);
  BEGIN
    Out.Char("("); Out.Long(LEN(a, 0), 0); Out.Char(")"); Out.Ln
  END proc1;

  PROCEDURE proc2(a: ARRAY OF ARRAY OF CHAR);
  BEGIN
    Out.Char("("); Out.Long(LEN(a, 0), 0); Out.String(", "); Out.Long(LEN(a, 1), 0); Out.Char(")"); Out.Ln
  END proc2;

BEGIN
  NEW(buf1, 100);
  proc1(buf1^);
  DISPOSE(buf1)
END DynamicArray1.
