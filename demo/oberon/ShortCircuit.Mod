MODULE ShortCircuit;

VAR i: INTEGER;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

(* Writes a string value to standard out. *)
PROCEDURE Write(str: STRING);
BEGIN
    printf(str)
END Write;

(* Writes a newline to standard out. *)
PROCEDURE WriteLn;
BEGIN
    printf("\n")
END WriteLn;

(* Writes an integer value to standard out. *)
PROCEDURE WriteInt(val: INTEGER);
BEGIN
    printf("%d", val)
END WriteInt;

PROCEDURE checkOne(i: INTEGER): BOOLEAN;
BEGIN
    Write("1 ");
    RETURN i # 0
END checkOne;

PROCEDURE checkTwo(i: INTEGER): BOOLEAN;
BEGIN
    Write("2 ");
    RETURN i > 0
END checkTwo;

PROCEDURE checkThree(i: INTEGER): BOOLEAN;
CONST Num = 100;
BEGIN
    Write("3 ");
    RETURN (Num DIV i = Num)
END checkThree;

PROCEDURE checkRange(i: INTEGER);
BEGIN
    IF (i >= 0) & (i < 10) OR (i >= 90) & (i < 100) THEN
        Write("passed")
    ELSE
        Write("not passed")
    END;
    WriteLn
END checkRange;

PROCEDURE check(i: INTEGER);
BEGIN
    WriteInt(i); Write(": ");
    IF checkOne(i) & checkTwo(i) & checkThree(i) THEN
        Write("-> passed")
    ELSE
        Write("-> not passed")
    END;
    WriteLn;
    WriteInt(i); Write(": ");
    IF checkOne(i) OR checkTwo(i) THEN
        Write("-> passed")
    ELSE
        Write("-> not passed")
    END;
    WriteLn;
    WriteInt(i); Write(": ");
    IF (checkOne(i) OR checkTwo(i)) & checkThree(i) THEN
        Write("-> passed")
    ELSE
        Write("-> not passed")
    END;
    WriteLn
END check;

BEGIN
    check(0);
    WriteLn;
    check(-1);
    WriteLn;
    check(1)
END ShortCircuit.

(* ./oberon0c ShortCircuit.Mod && opt -S -O3 ShortCircuit.ll | llc -O3 -o ShortCircuit.s && g++ ShortCircuit.s -o ShortCircuit && ./ShortCircuit *)
