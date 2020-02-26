MODULE HelloWorld;

CONST Message = "Hoi Niklaus!\n";

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

(* Writes a string value to standard out. *)
PROCEDURE Write(str: STRING);
BEGIN
    printf(str)
END Write;

BEGIN
	Write(Message)
END HelloWorld.