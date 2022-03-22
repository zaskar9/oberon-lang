MODULE Random;
IMPORT Oberon;

(* Import `rand` function from C <stdlib.h> library. *)
PROCEDURE rand(): INTEGER; EXTERN;

(* Import `srand` function from C <stdlib.h> library. *)
PROCEDURE srand(seed: LONGINT); EXTERN;

PROCEDURE Int*(max: INTEGER): INTEGER;
BEGIN
    RETURN rand() MOD max
END Int;

PROCEDURE Randomize*();
VAR time: LONGINT;
BEGIN
    srand(Oberon.Time())
END Randomize;

END Random.