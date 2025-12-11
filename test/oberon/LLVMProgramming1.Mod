MODULE LLVMProgramming1;

CONST x = 10;

VAR i: INTEGER;
    r: REAL;
    b: BOOLEAN;

BEGIN
  i := x;
  r := 5 * i;
  b := (i >= x) & (r / 2 < i)
END LLVMProgramming1.