(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** Module with math function operation on `REAL` type. *)
MODULE Math;

CONST pi* = 3.1415927410125732421875;
      e*  = 2.7182818284590450907956;


(* Declare `float sqrtf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] sqrtf(x: REAL): REAL; EXTERNAL [ "sqrtf" ];

(* Declare `float powf(float, float)` from C <math.h> library. *)
PROCEDURE [ "C" ] powf(base, exponent: REAL): REAL; EXTERNAL [ "powf" ];

(* Declare `float expf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] expf(x: REAL): REAL; EXTERNAL [ "expf" ];

(* Declare `float logf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] logf(x: REAL): REAL; EXTERNAL [ "logf" ];

(* Declare `float log2f(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] log2f(x: REAL): REAL; EXTERNAL [ "log2f" ];

(** Declare `float roundf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] roundf(x: REAL): REAL; EXTERNAL [ "roundf" ];

(* Declare `float sinf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] sinf(x: REAL): REAL; EXTERNAL [ "sinf" ];

(* Declare `float cosf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] cosf(x: REAL): REAL; EXTERNAL [ "cosf" ];

(* Declare `float tanf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] tanf(x: REAL): REAL; EXTERNAL [ "tanf" ];

(* Declare `float asinf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] asinf(x: REAL): REAL; EXTERNAL [ "asinf" ];

(* Declare `float acosf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] acosf(x: REAL): REAL; EXTERNAL [ "acosf" ];

(* Declare `float atanf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] atanf(x: REAL): REAL; EXTERNAL [ "atanf" ];

(* Declare `float atan2f(float, float)` from C <math.h> library. *)
PROCEDURE [ "C" ] atan2f(y, x: REAL): REAL; EXTERNAL [ "atan2f" ];

(* Declare `float sinhf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] sinhf(x: REAL): REAL; EXTERNAL [ "sinhf" ];

(* Declare `float coshf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] coshf(x: REAL): REAL; EXTERNAL [ "coshf" ];

(* Declare `float tanhf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] tanhf(x: REAL): REAL; EXTERNAL [ "tanhf" ];

(* Declare `float asinhf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] asinhf(x: REAL): REAL; EXTERNAL [ "asinhf" ];

(* Declare `float acoshf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] acoshf(x: REAL): REAL; EXTERNAL [ "acoshf" ];

(* Declare `float atanhf(float)` from C <math.h> library. *)
PROCEDURE [ "C" ] atanhf(x: REAL): REAL; EXTERNAL [ "atanhf" ];

(* Declare `float olang_math_realf(int32_t)` from Oberon runtime library. *)
PROCEDURE [ "C" ] realf(x: INTEGER): REAL; EXTERNAL [ "olang_math_realf" ];

(* Declare `int32_t olang_math_entierf(float)` from Oberon runtime library. *)
PROCEDURE [ "C" ] entierf(x: REAL): INTEGER; EXTERNAL [ "olang_math_entierf" ];


(** Computes the square root of x. *)
PROCEDURE sqrt*(x: REAL): REAL;
BEGIN
    RETURN sqrtf(x)
END sqrt;

(** Computes the the n-th power of x. *)
PROCEDURE power*(x, n: REAL): REAL;
BEGIN
    RETURN powf(x, n)
END power;

(** Computes e raised to the power of x. *)
PROCEDURE exp*(x: REAL): REAL;
BEGIN
    RETURN expf(x)
END exp;

(** Computes natural logarithm, i.e., the logarithm to base e, of x. *)
PROCEDURE ln*(x: REAL): REAL;
BEGIN
    RETURN logf(x)
END ln;

(** Computes the logarithm of x to the given base. *)
PROCEDURE log*(x, base: REAL): REAL;
BEGIN
    RETURN log2f(x) / log2f(base)
END log;

(** Rounds x to the nearest integer number. *)
PROCEDURE round*(x: REAL): REAL;
BEGIN
    RETURN roundf(x)
END round;

(** Computes the sine of x (measured in radians). *)
PROCEDURE sin*(x: REAL): REAL;
BEGIN
    RETURN sinf(x)
END sin;

(** Computes the cosine of x (measured in radians). *)
PROCEDURE cos*(x: REAL): REAL;
BEGIN
    RETURN cosf(x)
END cos;

(** Computes the tangent of x (measured in radians). *)
PROCEDURE tan*(x: REAL): REAL;
BEGIN
    RETURN tanf(x)
END tan;

(** Computes the principal value of the arc sine of x. *)
PROCEDURE arcsin*(x: REAL): REAL;
BEGIN
    RETURN asinf(x)
END arcsin;

(** Computes the principal value of the arc cosine of x. *)
PROCEDURE arccos*(x: REAL): REAL;
BEGIN
    RETURN acosf(x)
END arccos;

(** Computes the principal value of the arc tangent of x. *)
PROCEDURE arctan*(x: REAL): REAL;
BEGIN
    RETURN atanf(x)
END arctan;

(** Computes the arc tangent of y/x using the signs of arguments to determine the correct quadrant. *)
PROCEDURE arctan2*(x, y : REAL): REAL;
BEGIN
    RETURN atan2f(x, y)
END arctan2;

(* Computes hyperbolic sine of x. *)
PROCEDURE sinh*(x: REAL): REAL;
BEGIN
    RETURN sinhf(x)
END sinh;

(* Computes hyperbolic cosine of x. *)
PROCEDURE cosh*(x: REAL): REAL;
BEGIN
    RETURN coshf(x)
END cosh;

(* Computes hyperbolic tangent of x. *)
PROCEDURE tanh*(x: REAL): REAL;
BEGIN
    RETURN tanhf(x)
END tanh;

(** Computes the inverse hyperbolic sine of x. *)
PROCEDURE arcsinh*(x: REAL): REAL;
BEGIN
    RETURN asinhf(x)
END arcsinh;

(** Computes the inverse hyperbolic cosine of x. *)
PROCEDURE arccosh*(x: REAL): REAL;
BEGIN
    RETURN acoshf(x)
END arccosh;

(** Computes the inverse hyperbolic tangent of x. *)
PROCEDURE arctanh*(x: REAL): REAL;
BEGIN
    RETURN atanhf(x)
END arctanh;

(** Converts an integer value x to a floating-point value. *)
PROCEDURE real*(x: INTEGER): REAL;
BEGIN
    RETURN realf(x)
END real;

(** Computes the largest integer value not greater than x. *)
PROCEDURE entier*(x: REAL): INTEGER;
BEGIN
    RETURN entierf(x)
END entier;

END Math.
