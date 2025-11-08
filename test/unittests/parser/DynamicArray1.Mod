MODULE DynamicArray1;

  TYPE
    Buffer = POINTER TO ARRAY OF BYTE;

  VAR
    buf1: Buffer;
    buf2: POINTER TO ARRAY OF BYTE;

BEGIN
  NEW(buf1, 100);
  NEW(buf2, 200)
END DynamicArray1.
