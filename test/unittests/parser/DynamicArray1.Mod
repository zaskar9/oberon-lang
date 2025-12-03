(*
  RUN: %oberon --std=Oberon2 --run %s
*)
MODULE DynamicArray1;

  TYPE
    Buffer = POINTER TO ARRAY OF CHAR;
    Procedure = PROCEDURE(a: ARRAY OF CHAR);

  VAR
    buf1: Buffer;
    buf2: POINTER TO ARRAY OF CHAR;
    buf3: POINTER TO ARRAY OF ARRAY OF CHAR;
    buf4: POINTER TO ARRAY OF ARRAY 8 OF CHAR;

  PROCEDURE proc1(a: ARRAY OF CHAR);
  END proc1;

  PROCEDURE proc2(a: ARRAY OF ARRAY OF CHAR);
  END proc2;

END DynamicArray1.
