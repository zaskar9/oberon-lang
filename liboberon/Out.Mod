MODULE Out;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

(* Import `void rt_out_int(long, int)` from Oberon runtime library. *)
PROCEDURE rt_out_int(i: LONGINT; n: INTEGER); EXTERN;

(* Import `void rt_out_real(float, int)` from Oberon runtime library. *)
PROCEDURE rt_out_real(x: REAL; n: INTEGER); EXTERN;

(* Writes a string value to standard out. *)
PROCEDURE String*(str: STRING);
BEGIN
    printf(str)
END String;

(* Writes a newline to standard out. *)
PROCEDURE Ln*;
BEGIN
    printf("\n")
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
    printf("%E", val)
END LongReal;

PROCEDURE Set*(set: SET);
VAR i: INTEGER;
BEGIN
    String("{ ");
    FOR i := 0 TO 31 DO
        IF i IN set THEN
            Int(i, 0); String(" ")
        END
    END;
    String("}")
END Set;

END Out.