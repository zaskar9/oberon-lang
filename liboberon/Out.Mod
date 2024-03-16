MODULE Out;

CONST nl* = "\n";

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: ARRAY OF CHAR; ...): INTEGER; EXTERN;

(* Import `putchar` function from C <stdio.h> library. *)
PROCEDURE putchar(ch: CHAR): INTEGER; EXTERN;

(* Import `void rt_out_int(long, int)` from Oberon runtime library. *)
PROCEDURE rt_out_int(i: LONGINT; n: INTEGER); EXTERN;

(* Import `void rt_out_real(float, int)` from Oberon runtime library. *)
PROCEDURE rt_out_real(x: REAL; n: INTEGER); EXTERN;

(* Writes a character to standard out. *)
PROCEDURE Char*(ch: CHAR);
BEGIN
    putchar(ch)
END Char;

(* Writes a string to standard out. *)
PROCEDURE String*(s: ARRAY OF CHAR);
VAR i: LONGINT;
BEGIN
    i := 0;
    WHILE (i < LEN(s)) & (s[i] # "\0") DO
        Char(s[i]);
        INC(i)
    END
END String;

(* Writes a newline to standard out. *)
PROCEDURE Ln*;
BEGIN
    Char(nl)
END Ln;

(* Write the integer `val` in `n` field positions. *)
PROCEDURE Int*(val: INTEGER; n: INTEGER);
VAR l : LONGINT;
BEGIN
    l := val;
    rt_out_int(l, n)
END Int;

(* Write the long integer `val` in `n` field positions. *)
PROCEDURE Long*(val: LONGINT; n: INTEGER);
BEGIN
    rt_out_int(val, n)
END Long;

PROCEDURE Hex*(x: INTEGER);
BEGIN
    printf("%08X", x)
END Hex;

PROCEDURE LongHex*(x: LONGINT);
BEGIN
    printf("%016llX", x)
END LongHex;

PROCEDURE Real*(x: REAL; n: INTEGER);
BEGIN
    rt_out_real(x, n)
END Real;

PROCEDURE LongReal*(val: LONGREAL);
BEGIN
    printf("%.16G", val)
END LongReal;

PROCEDURE Set*(set: SET);
VAR i: INTEGER;
BEGIN
    String("{ ");
    FOR i := 0 TO 31 DO
        IF i IN set THEN
            Int(i, 0); Char(" ")
        END
    END;
    Char("}")
END Set;

END Out.