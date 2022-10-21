MODULE Out;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

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

(* Writes an integer value to standard out. *)
PROCEDURE Int*(val: INTEGER);
BEGIN
    printf("%d", val)
END Int;

PROCEDURE Long*(val: LONGINT);
BEGIN
    printf("%ld", val)
END Long;

PROCEDURE Real*(val: REAL);
VAR valL: LONGREAL;
BEGIN
    valL := val;
    printf("%f", valL)
END Real;

END Out.