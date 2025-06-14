(*
  RUN: %oberon --run %s
*)
MODULE ArrayLen1;

CONST Dim = 10;
      Len = 12;
      Msg = "Hello World!";

TYPE Vector = ARRAY Dim OF INTEGER;
     Matrix = ARRAY Dim, 2 * Dim OF INTEGER;
     Cube = ARRAY Dim, 2 * Dim, 3 * Dim OF INTEGER;

VAR v: Vector;

PROCEDURE Assert1(a: ARRAY OF INTEGER; i: INTEGER);
BEGIN
    ASSERT(LEN(a) = i)
END Assert1;

PROCEDURE Assert2(a: ARRAY OF ARRAY OF INTEGER; i, j: INTEGER);
BEGIN
    ASSERT(LEN(a, 0) = i);
    ASSERT(LEN(a, 1) = j)
END Assert2;

PROCEDURE Assert3(a: ARRAY OF ARRAY OF ARRAY OF INTEGER; i, j, k: INTEGER);
BEGIN
    ASSERT(LEN(a, 0) = i);
    ASSERT(LEN(a, 1) = j);
    ASSERT(LEN(a, 2) = k)
END Assert3;

PROCEDURE TestLen;
VAR v: Vector;
    m: Matrix;
    c: Cube;
BEGIN
    ASSERT(LEN(v) = Dim);
    Assert1(v, Dim);
    ASSERT(LEN(m, 0) = Dim);
    ASSERT(LEN(m, 1) = 2 * Dim);
    Assert1(m[0], 2 * Dim);
    Assert2(m, Dim, 2 * Dim);
    ASSERT(LEN(c, 0) = Dim);
    ASSERT(LEN(c, 1) = 2 * Dim);
    ASSERT(LEN(c, 2) = 3 * Dim);
    Assert1(c[0][0], 3 * Dim);
    Assert2(c[0], 2 * Dim, 3 * Dim);
    Assert3(c, Dim, 2 * Dim, 3 * Dim)
END TestLen;

PROCEDURE StrLen(str: ARRAY OF CHAR): INTEGER;
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE (i < LEN(str)) & (str[i] # 0X) DO
        INC(i)
    END;
    RETURN i
END StrLen;

PROCEDURE TestStrLen(str: ARRAY OF CHAR);
BEGIN
    ASSERT(LEN(Msg) = StrLen(Msg));
    ASSERT(LEN(str) = StrLen(Msg) + 1)
END TestStrLen;

PROCEDURE TestMixing();
  VAR a: ARRAY Dim OF INTEGER;
  PROCEDURE Call(a: Vector); END Call;
BEGIN
    Call(a)
END TestMixing;

PROCEDURE ForwardAssert1(a: ARRAY OF INTEGER; i: INTEGER);
BEGIN
    Assert1(a, i)
END ForwardAssert1;

PROCEDURE TestPassingOn;
VAR a: ARRAY Dim OF INTEGER;
BEGIN
    ForwardAssert1(a, Dim)
END TestPassingOn;

BEGIN
    ASSERT(LEN(v) = Dim);
    Assert1(v, Dim);
    TestLen;
    ASSERT(LEN(Msg) = Len);
    TestStrLen(Msg);
    TestPassingOn
END ArrayLen1.