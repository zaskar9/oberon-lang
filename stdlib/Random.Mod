(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** Module implementing a pseudo-random number generator *)
MODULE Random;
IMPORT Oberon;


(* Declare `int rand()` function from C <stdlib.h> library. *)
PROCEDURE [ "C" ] RandomInt(): INTEGER; EXTERNAL [ "rand" ];

(* Declare `void srand(long)` function from C <stdlib.h> library. *)
PROCEDURE [ "C" ] Seed(seed: LONGINT); EXTERNAL [ "srand" ];


(** Returns next random integer number from 0 to max *)
PROCEDURE Int*(max: INTEGER): INTEGER;
BEGIN
    RETURN RandomInt() MOD max
END Int;

(** Reset the random process with the current time *)
PROCEDURE Randomize*();
VAR time: LONGINT;
BEGIN
    Seed(Oberon.Time())
END Randomize;

END Random.