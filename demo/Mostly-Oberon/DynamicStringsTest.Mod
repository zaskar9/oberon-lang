(**DynamicStringsTest.Mod provides an implementation of dynamic 
strings using a linked list of records holding ARRAY OF CHAR. *)
MODULE DynamicStringsTest;
  IMPORT DynamicStrings, Strings, Out, Convert := extConvert;

PROCEDURE AssertTrue(expr : BOOLEAN; msg : ARRAY OF CHAR; VAR errCnt : INTEGER);
BEGIN
  IF expr # TRUE THEN
    Out.String(msg);Out.Ln;
    INC(errCnt);
  END;
END AssertTrue;

PROCEDURE TestBasic() : BOOLEAN;
  VAR ok, b, ignore : BOOLEAN; 
    s, s1, s2 : DynamicStrings.DynamicString;
    a1, a2, outMsg : ARRAY 2048 OF CHAR;
    c1, c2 : ARRAY 2 OF CHAR;
    ith : ARRAY 24 OF CHAR;
    i, errCnt : INTEGER;
BEGIN
  errCnt := 0;
  a1 := "Hello World!";
  a2 := "";
  ok := TRUE;
  DynamicStrings.New(s);
  AssertTrue(s # NIL, "s should not be NIL", errCnt);
  AssertTrue(DynamicStrings.Length(s) = 0, "s should be empty now.", errCnt);
  DynamicStrings.Set(s, a1);
  AssertTrue(DynamicStrings.Length(s) = Strings.Length(a1), "s should have length of a1", errCnt);
  DynamicStrings.ToCharArray(s, a2, b);
  AssertTrue(b, "ToCharArray(s, a2, b) should b should be TRUE", errCnt);
  AssertTrue(a1 = a2, "expected a1 = a2", errCnt);
  ASSERT(errCnt = 0);

  a1 := "Hello World!";
  a2 := "";
  c1 := "";
  c2 := "";
  FOR i := 0 TO Strings.Length(a1) DO
    Strings.Extract(a1, i, 1, c1);
    Convert.IntToString(i, ith, ignore);
    DynamicStrings.GetChar(s, i, c2);
    outMsg := ""; Strings.Append("expected (", outMsg);
    Strings.Append(ith, outMsg); Strings.Append(") ->", outMsg);
    Strings.Append(c1, outMsg); Strings.Append("<-, got ->", outMsg);
    Strings.Append(c2, outMsg); Strings.Append("<-", outMsg);
    AssertTrue(b, outMsg, errCnt);
    AssertTrue(c1 = c2, outMsg, errCnt);
    ASSERT(errCnt = 0);
  END;

  a1 := "Hello World!";
  DynamicStrings.Set(s, a1);
  FOR i := 0 TO Strings.Length(a1) DO
    Convert.IntToString(i, ith, ignore);
    DynamicStrings.SetChar(s, i, "X");
    DynamicStrings.GetChar(s, i, c2);
    outMsg := "";
    Strings.Append("expected X at a1[", outMsg);
    Strings.Append(ith, outMsg);Strings.Append("], got >", outMsg);
    Strings.Append(c2, outMsg);Strings.Append("<", outMsg);
    AssertTrue(c2 = "X", outMsg, errCnt);
  END;

  DynamicStrings.New(s1); DynamicStrings.Set(s1, "0123456789");
  DynamicStrings.New(s2); DynamicStrings.Set(s2, "<-------->");
  a1 := "";
  a2 := "0123456789";

  FOR i := 0 TO 24 DO
    DynamicStrings.Append(s2, s1);
    Strings.Append("<-------->", a2);
  END;
  DynamicStrings.ToCharArray(s1, a1, ok);
  AssertTrue(ok, "ToCharArray(s1, a1, ok) returned FALSE", errCnt);

  outMsg := ""; Strings.Append("expected (a2) '", outMsg);
  Strings.Append(a2, outMsg);Strings.Append("', got (a1) '", outMsg);
  Strings.Append(a1, outMsg);Strings.Append("'", outMsg);
  AssertTrue(a1 = a2, outMsg, errCnt);

  IF (errCnt # 0) THEN
    Out.String("TestBasic error count: ");Out.Int(errCnt, 0);Out.Ln;
  END;
  RETURN (errCnt = 0)
END TestBasic;

BEGIN
  ASSERT(TestBasic());
  Out.String("Success!");Out.Ln;
END DynamicStringsTest.
