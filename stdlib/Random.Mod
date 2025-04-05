(** Module with apseudo-random number generator *)
MODULE Random;
IMPORT Oberon;

(* Import `rand` function from C <stdlib.h> library. *)
PROCEDURE rand(): INTEGER; EXTERN;

(* Import `srand` function from C <stdlib.h> library. *)
PROCEDURE srand(seed: LONGINT); EXTERN;

(** Returns next random integer number from 0 to max *)
PROCEDURE Int*(max: INTEGER): INTEGER;
BEGIN
    RETURN rand() MOD max
END Int;

(** Reset the random process with the current time *)
PROCEDURE Randomize*();
VAR time: LONGINT;
BEGIN
    srand(Oberon.Time())
END Randomize;

END Random.