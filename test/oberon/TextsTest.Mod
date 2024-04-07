MODULE TextsTest;

  IMPORT T := Texts, Reals, Out;

  CONST
    REALMIN = 1.175494351E-38;
    REALMAX = 3.402823466E+38;
    LONGREALMIN = -1.7976931348623158E308;
    LONGREALMAX = -2.2250738585072014E-308;

  VAR
    W: T.Writer;
    s: ARRAY 20 OF CHAR;

BEGIN
  T.WriteString(W, "Hello World");        T.WriteLn(W);
  T.WriteInt(W, MIN(SHORTINT), 25);       T.WriteInt(W, MAX(SHORTINT), 25); T.WriteLn(W);
  T.WriteInt(W, MIN(INTEGER), 25);        T.WriteInt(W, MAX(INTEGER), 25); T.WriteLn(W);
  T.WriteInt(W, MIN(LONGINT), 25);        T.WriteInt(W, MAX(LONGINT), 25); T.WriteLn(W);

  T.WriteReal(W, REALMIN, 25);            T.WriteReal(W, REALMAX, 25); T.WriteLn(W);
  T.WriteReal(W, MIN(REAL), 25);          T.WriteReal(W, MAX(REAL), 25); T.WriteLn(W);
  T.WriteReal(W, Reals.NaN(), 25);        T.WriteReal(W, 0, 25); T.WriteLn(W);

  T.WriteLongReal(W, LONGREALMIN, 25);    T.WriteLongReal(W, LONGREALMAX, 25); T.WriteLn(W);
  T.WriteLongReal(W, MIN(LONGREAL), 25);  T.WriteLongReal(W, MAX(LONGREAL), 25); T.WriteLn(W);
  T.WriteLongReal(W, Reals.NaNL(), 25);   T.WriteLongReal(W, 0, 25); T.WriteLn(W);

  T.WriteShortHex(W, MIN(SHORTINT));      T.Write(W, "\t");
  T.WriteHex(W, MIN(INTEGER));            T.Write(W, "\t");
  T.WriteLongHex(W, MIN(LONGINT));        T.WriteLn(W);

  T.WriteShortHex(W, MAX(SHORTINT));      T.Write(W, "\t");
  T.WriteHex(W, MAX(INTEGER));            T.Write(W, "\t");
  T.WriteLongHex(W, MAX(LONGINT));        T.WriteLn(W);

  T.WriteShortHex(W, -1);                 T.Write(W, "\t");
  T.WriteHex(W, -1);                      T.Write(W, "\t");
  T.WriteLongHex(W, -1);                  T.WriteLn(W)
END TextsTest.