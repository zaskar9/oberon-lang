(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE ByteType;
IMPORT Out;

TYPE QWORD = ARRAY 8 OF BYTE;
     DWORD = ARRAY 4 OF BYTE;
     WORD = ARRAY 2 OF BYTE;

VAR x: BYTE;
    i: INTEGER;
    l: LONGINT;
    q: QWORD;

PROCEDURE EncInt64(num: LONGINT; VAR qword: QWORD);
VAR i: INTEGER;
BEGIN
    i := 0;
    REPEAT
        qword[i] := num MOD 100H;
        num := num DIV 100H;
        INC(i)
    UNTIL i = LEN(qword)
END EncInt64;

PROCEDURE DecInt64(VAR num: LONGINT; qword: QWORD);
VAR i: INTEGER;
BEGIN
    i := 0;
    num := 0;
    REPEAT
        num := num + LSL(qword[i], i * 8);
        INC(i)
    UNTIL i = LEN(qword)
END DecInt64;

BEGIN
    x := 255;
    ASSERT(x = 255);
    l := x;
    ASSERT(l = x);
    Out.Long(x, 0); Out.Ln;
    Out.Long(l, 0); Out.Ln;

    EncInt64(-1, q);
    DecInt64(l, q);
    Out.Long(l, 20); Out.Ln;

    EncInt64(MIN(LONGINT), q);
    DecInt64(l, q);
    Out.Long(l, 20); Out.Ln;

    EncInt64(MAX(LONGINT), q);
    DecInt64(l, q);
    Out.Long(l, 20); Out.Ln;

END ByteType.
(*
  CHECK: 255
  CHECK: 255
  CHECK:                   -1
  CHECk: -9223372036854775808
  CHECK:  9223372036854775807
*)