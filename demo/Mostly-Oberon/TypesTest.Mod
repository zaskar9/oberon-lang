(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
MODULE TypesTest;
  IMPORT Types, T := Tests;

VAR
    ts : T.TestSet;

PROCEDURE TestIsA() : BOOLEAN;
  VAR test, expected, got : BOOLEAN;
BEGIN test := TRUE;
  expected := TRUE;
  got := Types.IsCharDigit("7");
  T.ExpectedBool(expected, got, "Types.IsDigit(7)", test);

  expected := FALSE;
  got := Types.IsCharDigit("-");
  T.ExpectedBool(expected, got, "Types.IsDigit(-)", test);
  got := Types.IsCharDigit("q");
  T.ExpectedBool(expected, got, "Types.IsDigit(q)", test);

  RETURN test
END TestIsA;

PROCEDURE TestDigitHandling() : BOOLEAN;
  VAR test : BOOLEAN; i, j : INTEGER; ch : CHAR;
BEGIN test := TRUE;
  i := 0;
  WHILE (i < 10) & test DO
    ch := Types.DigitToChar(i, test);
    test := test & Types.IsCharDigit(ch);
    INC(i);
  END;
  i := ORD("0");
  WHILE (i <= ORD("9")) & test DO
    j := Types.CharToDigit(CHR(i), test);
    test := test & Types.IsIntDigit(j);
    INC(i);
  END;
  RETURN test
END TestDigitHandling;

PROCEDURE TestIntegerShifts() : BOOLEAN;
  VAR test: BOOLEAN; x, a, b : INTEGER;
BEGIN test := TRUE;
  x := 1234; a := 0; b := 0;
  T.ExpectedInt(x, 1234, "x should be 1234", test);
  a := Types.IntShiftRight(x, b);
  T.ExpectedInt(a, 123, "a should be 123", test);
  T.ExpectedInt(b, 4, "b should be 4", test);

  x := a;
  T.ExpectedInt(x, 123, "x should be 123", test);
  a := Types.IntShiftRight(x, b);
  T.ExpectedInt(a, 12, "a should be 12", test);
  T.ExpectedInt(b, 3, "b should be 3", test);

  x := a;
  T.ExpectedInt(x, 12, "x should be 12", test);
  a := Types.IntShiftRight(x, b);
  T.ExpectedInt(a, 1, "a should be 1", test);
  T.ExpectedInt(b, 2, "b should be 2", test);

  x := a;
  T.ExpectedInt(x, 1, "x should be 1", test);
  a := Types.IntShiftRight(x, b);
  T.ExpectedInt(a, 0, "a should be 0", test);
  T.ExpectedInt(b, 1, "b should be 1", test);

  x := a;
  T.ExpectedInt(x, 0, "x should be 0", test);
  a := Types.IntShiftRight(x, b);
  T.ExpectedInt(a, 0, "a should be 0", test);
  T.ExpectedInt(b, 0, "b should be 0", test);

  a := 0; b := 1; 
  x := Types.IntShiftLeft(a, b);
  T.ExpectedInt(x, 1, "x should be 1", test);

  a := x;  b := 2;
  x := Types.IntShiftLeft(a, b);
  T.ExpectedInt(x, 12, "x should be 12", test);

  a := x;  b := 3;
  x := Types.IntShiftLeft(a, b);
  T.ExpectedInt(x, 123, "x should be 123", test);

  a := x;  b := 4;
  x := Types.IntShiftLeft(a, b);
  T.ExpectedInt(x, 1234, "x should be 1234", test);

  RETURN test
END TestIntegerShifts;

PROCEDURE TestItoa() : BOOLEAN;
  VAR test, ok : BOOLEAN; expected, got : ARRAY 256 OF CHAR;
      src : INTEGER;
BEGIN test := TRUE;
  src := 1;
  expected := "1";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(1, got, ok)", test);

  src := -1;
  expected := "-1";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(-1, got, ok)", test);

  src := 12;
  expected := "12";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(12, got, ok)", test);

  src := -12;
  expected := "-12";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(-12, got, ok)", test);
  
  src := 120;
  expected := "120";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(120, got, ok)", test);

  src := 1200;
  expected := "1200";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(1200, got, ok)", test);

  src := -1234;
  expected := "-1234";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(-1234, got, ok)", test);

  src := 1234567890;
  expected := "1234567890";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(1234567890, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Itoa(1234567890, got,ok), ok = TRUE, expected no overflow", test);

  src := -1234567890;
  expected := "-1234567890";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(-1234567890, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Itoa(-1234567890, got,ok), ok = TRUE, expected no overflow", test);

  src := 2147483647;
  expected := "2147483647";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(2147483647, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Itoa(2147483647, got, ok) ok = TRUE, expected no overflow", test);

  src := -2147483647;
  expected := "-2147483647";
  Types.Itoa(src, got, ok);
  T.ExpectedString(expected, got, "Itoa(-2147483647, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Itoa(-2147483647, got, ok) should not have detected overflow, ok = TRUE", test);

  RETURN test
END TestItoa;

PROCEDURE TestAtoi() : BOOLEAN;
  VAR test, ok : BOOLEAN; expected, got : INTEGER;
      src : ARRAY 256 OF CHAR;
BEGIN test := TRUE;
  src := "1";
  expected := 1;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`1`, got, ok)", test);

  src := "+1";
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`+1`, got, ok)", test);

  src := "-1";
  expected := -1;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`-1`, got, ok)", test);
  src := "12";
  expected := 12;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`12`, got, ok)", test);

  src := "+12";
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`12`, got, ok)", test);

  src := "-12";
  expected := -12;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`-12`, got, ok)", test);

  src := "-1234";
  expected := -1234;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`-1234`, got, ok)", test);
  
  src := "200";
  expected := 200;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`200`, got, ok)", test);

  src := "1234567890";
  expected := 1234567890;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`1234567890`, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Atoi(`1234567890`, got, ok), ok = TRUE, expected no overflow", test);

  src := "-1234567890";
  expected := -1234567890;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`-1234567890`, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Atoi(`-1234567890`, got, ok), ok = TRUE, expected no overflow", test);

  src := "2147483647";
  expected := 2147483647;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`2147483647`, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Atoi(`2147483647`, got, ok) ok = TRUE, expected no overflow", test);

  src := "-2147483647";
  expected := -2147483647;
  Types.Atoi(src, got, ok);
  T.ExpectedInt(expected, got, "Atoi(`-2147483647`, got, ok)", test);
  T.ExpectedBool(TRUE, ok, "Atoi(`-2147483647`, got, ok) should not have detected overflow, ok = TRUE", test);

  (* NOTE: where the integer gets beyond range the compiler should
     detect and abort compilation *)
  RETURN test
END TestAtoi;


BEGIN
  T.Init(ts, "Test Types module");
  T.Add(ts, TestIsA, "Test IsA");
  T.Add(ts, TestDigitHandling, "Test Digit Handling");
  T.Add(ts, TestIntegerShifts, "Test Integer Shifts");
  T.Add(ts, TestItoa, "Test Itoa");
  T.Add(ts, TestAtoi, "Test Atoi");
  ASSERT(T.Run(ts));
END TypesTest.
