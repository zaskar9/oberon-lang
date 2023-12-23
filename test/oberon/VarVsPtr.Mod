MODULE VarVsPtr;
IMPORT Out;

TYPE IntPtr = POINTER TO INTEGER;

VAR a, b: INTEGER;
    aptr, bptr: IntPtr;

PROCEDURE Swap1(VAR a, b: IntPtr);
  VAR tmp: IntPtr;
  BEGIN
    tmp := a;
    a := b;
    b := tmp
  END Swap1;

PROCEDURE Swap2(a, b: IntPtr);
  VAR tmp: IntPtr;
  BEGIN
    tmp := a;
    a := b;
    b := tmp
  END Swap2;

PROCEDURE Swap3(VAR a, b: INTEGER);
  VAR tmp: INTEGER;
  BEGIN
    tmp := a;
    a := b;
    b := tmp
  END Swap3;

PROCEDURE Swap4(a, b: INTEGER);
  VAR tmp: INTEGER;
  BEGIN
    tmp := a;
    a := b;
    b := tmp
  END Swap4;

BEGIN
  NEW(aptr); aptr^ := 42;
  NEW(bptr); bptr^ := 24;
  Swap1(aptr, bptr);
  ASSERT((aptr^ = 24) & (bptr^ = 42));
  Swap2(aptr, bptr);
  ASSERT((aptr^ = 24) & (bptr^ = 42));
  FREE(aptr);
  FREE(bptr);
  a := 42;
  b := 24;
  Swap3(a, b);
  ASSERT((a = 24) & (b = 42));
  Swap4(a, b);
  ASSERT((a = 24) & (b = 42));
  Out.String("[OK] assertions passed!"); Out.Ln
END VarVsPtr.
