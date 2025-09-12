(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** Module with math function operation on `LONGREAL` type. *)
MODULE MathL;

CONST pi* = 3.141592653589793115997963468544185161590576171875E64 * 1.0E-64;
      e*  = 2.718281828459045090795598298427648842334747314453E64 * 1.0E-64;

(* CONST pi* = 3.14159265358979323846E39 * 1.0E-39;
      e*  = 2.71828182845904523536E39 * 1.0E-39; *)


(* Declare `double sqrt(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] sqrtd(x: LONGREAL): LONGREAL; EXTERNAL [ "sqrt" ];

(* Declare `double pow(double, double)` from C <math.h> library. *)
PROCEDURE [ "C" ] powd(base, exponent: LONGREAL): LONGREAL; EXTERNAL [ "pow" ];

(* Declare `double exp(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] expd(x: LONGREAL): LONGREAL; EXTERNAL [ "exp" ];

(* Declare `double log(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] logd(x: LONGREAL): LONGREAL; EXTERNAL [ "log" ];

(* Declare `double log2(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] log2d(x: LONGREAL): LONGREAL; EXTERNAL [ "log2" ];

(** Declare `double round(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] roundd(x: LONGREAL): LONGREAL; EXTERNAL [ "round" ];

(* Declare `double sin(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] sind(x: LONGREAL): LONGREAL; EXTERNAL [ "sin" ];

(* Declare `double cos(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] cosd(x: LONGREAL): LONGREAL; EXTERNAL [ "cos" ];

(* Declare `double tan(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] tand(x: LONGREAL): LONGREAL; EXTERNAL [ "tan" ];

(* Declare `double asin(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] asind(x: LONGREAL): LONGREAL; EXTERNAL [ "asin" ];

(* Declare `double acos(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] acosd(x: LONGREAL): LONGREAL; EXTERNAL [ "acos" ];

(* Declare `double atan(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] atand(x: LONGREAL): LONGREAL; EXTERNAL [ "atan" ];

(* Declare `double atan2(double, double)` from C <math.h> library. *)
PROCEDURE [ "C" ] atan2d(y, x: LONGREAL): LONGREAL; EXTERNAL [ "atan2" ];

(* Declare `double sinh(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] sinhd(x: LONGREAL): LONGREAL; EXTERNAL [ "sinh" ];

(* Declare `double cosh(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] coshd(x: LONGREAL): LONGREAL; EXTERNAL [ "cosh" ];

(* Declare `double tanh(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] tanhd(x: LONGREAL): LONGREAL; EXTERNAL [ "tanh" ];

(* Declare `double asinh(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] asinhd(x: LONGREAL): LONGREAL; EXTERNAL [ "asinh" ];

(* Declare `double acosh(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] acoshd(x: LONGREAL): LONGREAL; EXTERNAL [ "acosh" ];

(* Declare `double atanh(double)` from C <math.h> library. *)
PROCEDURE [ "C" ] atanhd(x: LONGREAL): LONGREAL; EXTERNAL [ "atanh" ];

(* Declare `double olang_mathL_real(int64_t)` from Oberon runtime library. *)
PROCEDURE [ "C" ] reald(x: LONGINT): LONGREAL; EXTERNAL [ "olang_mathL_real" ];

(* Declare `int64_t olang_mathL_entier(double)` from Oberon runtime library. *)
PROCEDURE [ "C" ] entierd(x: LONGREAL): LONGINT; EXTERNAL [ "olang_mathL_entier" ];


(** Computes the square root of x. *)
PROCEDURE sqrt*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN sqrtd(x)
END sqrt;

(** Computes the the n-th power of x. *)
PROCEDURE power*(x, n: LONGREAL): LONGREAL;
BEGIN
    RETURN powd(x, n)
END power;

(** Computes e raised to the power of x. *)
PROCEDURE exp*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN expd(x)
END exp;

(** Computes natural logarithm, i.e., the logarithm to base e, of x. *)
PROCEDURE ln*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN logd(x)
END ln;

(** Computes the logarithm of x to the given base. *)
PROCEDURE log*(x, base: LONGREAL): LONGREAL;
BEGIN
    RETURN log2d(x) / log2d(base)
END log;

(** Rounds x to the nearest integer number. *)
PROCEDURE round*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN roundd(x)
END round;

(** Computes the sine of x (measured in radians). *)
PROCEDURE sin*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN sind(x)
END sin;

(** Computes the cosine of x (measured in radians). *)
PROCEDURE cos*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN cosd(x)
END cos;

(** Computes the tangent of x (measured in radians). *)
PROCEDURE tan*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN tand(x)
END tan;

(** Computes the principal value of the arc sine of x. *)
PROCEDURE arcsin*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN asind(x)
END arcsin;

(** Computes the principal value of the arc cosine of x. *)
PROCEDURE arccos*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN acosd(x)
END arccos;

(** Computes the principal value of the arc tangent of x. *)
PROCEDURE arctan*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN atand(x)
END arctan;

(** Computes the arc tangent of y/x using the signs of arguments to determine the correct quadrant. *)
PROCEDURE arctan2*(x, y : LONGREAL): LONGREAL;
BEGIN
    RETURN atan2d(x, y)
END arctan2;

(* Computes hyperbolic sine of x. *)
PROCEDURE sinh*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN sinhd(x)
END sinh;

(* Computes hyperbolic cosine of x. *)
PROCEDURE cosh*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN coshd(x)
END cosh;

(* Computes hyperbolic tangent of x. *)
PROCEDURE tanh*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN tanhd(x)
END tanh;

(** Computes the inverse hyperbolic sine of x. *)
PROCEDURE arcsinh*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN asinhd(x)
END arcsinh;

(** Computes the inverse hyperbolic cosine of x. *)
PROCEDURE arccosh*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN acoshd(x)
END arccosh;

(** Computes the inverse hyperbolic tangent of x. *)
PROCEDURE arctanh*(x: LONGREAL): LONGREAL;
BEGIN
    RETURN atanhd(x)
END arctanh;

(** Converts an integer value x to a floating-point value. *)
PROCEDURE real*(x: LONGINT): LONGREAL;
BEGIN
    RETURN reald(x)
END real;

(** Computes the largest integer value not greater than x. *)
PROCEDURE entier*(x: LONGREAL): LONGINT;
BEGIN
    RETURN entierd(x)
END entier;

END MathL.
