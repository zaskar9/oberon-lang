MODULE Out;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

(* Import `rt_out_int` from Oberon runtime library. *)
PROCEDURE rt_out_int(i: LONGINT; n: INTEGER); EXTERN;

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
BEGIN
    rt_out_int(val, n)
END Int;

(* Write the long integer `val` in `n` field positions. *)
PROCEDURE Long*(val: LONGINT; n: INTEGER);
BEGIN
    rt_out_int(val, n)
END Long;

PROCEDURE Real*(val: REAL);
VAR valL: LONGREAL;
BEGIN
    valL := val;
    printf("%f", valL)
END Real;

END Out.