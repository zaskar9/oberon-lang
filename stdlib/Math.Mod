(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** Module with math function operation on `REAL` type. *)
MODULE Math;

CONST e* = 2.7182818284;
      pi* = 3.1415926535;


(* Declare `float sqrtf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] sqrtf(x: REAL): REAL; EXTERNAL [ "sqrtf" ];

(* Declare `float expf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] expf(x: REAL): REAL; EXTERNAL [ "expf" ];

(* Declare `float logf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] logf(x: REAL): REAL; EXTERNAL [ "logf" ];

(* Declare `float sinf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] sinf(x: REAL): REAL; EXTERNAL [ "sinf" ];

(* Declare `float cosf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] cosf(x: REAL): REAL; EXTERNAL [ "cosf" ];

(* Declare `float atanf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] atanf(x: REAL): REAL; EXTERNAL [ "atanf" ];

(* Declare `float rt_realf(float)` from Oberon runtime library. *)
PROCEDURE [ "C" ] realf(x: INTEGER): REAL; EXTERNAL [ "olang_math_realf" ];

(* Declare `int32_t rt_entierf(float)` from Oberon runtime library. *)
PROCEDURE [ "C" ] entierf(x: REAL): INTEGER; EXTERNAL [ "olang_math_entierf" ];


(** Computes the square root of the `REAL` x *)
PROCEDURE sqrt*(x: REAL): REAL;
BEGIN
    RETURN sqrtf(x)
END sqrt;

(** Computes e raised to the power of x *)
PROCEDURE exp*(x: REAL): REAL;
BEGIN
    RETURN expf(x)
END exp;

(** Computes natural (e) logarithm of x *)
PROCEDURE ln*(x: REAL): REAL;
BEGIN
    RETURN logf(x)
END ln;

(** Computes the sine of the angle `REAL` x in radians *)
PROCEDURE sin*(x: REAL): REAL;
BEGIN
    RETURN sinf(x)
END sin;

(** Computes the cosine of the angle `REAL` x in radians *)
PROCEDURE cos*(x: REAL): REAL;
BEGIN
    RETURN cosf(x)
END cos;

(** Computes the arc tangent of the value `REAL` x *)
PROCEDURE arctan*(x: REAL): REAL;
BEGIN
    RETURN atanf(x)
END arctan;

(** Return the `INTEGER` x converted to `REAL` *)
PROCEDURE real*(x: INTEGER): REAL;
BEGIN
    RETURN realf(x)
END real;

(** Computes the largest integer value not greater than x *)
PROCEDURE entier*(x: REAL): INTEGER;
BEGIN
    RETURN entierf(x)
END entier;

END Math.
