(** Module with math function operation on `REAL` type. *)
MODULE Math;

CONST e* = 2.7182818284;
      pi* = 3.1415926535;

PROCEDURE sqrtf(x: REAL): REAL; EXTERN;
PROCEDURE expf(x: REAL): REAL; EXTERN;
PROCEDURE logf(x: REAL): REAL; EXTERN;
PROCEDURE sinf(x: REAL): REAL; EXTERN;
PROCEDURE cosf(x: REAL): REAL; EXTERN;
PROCEDURE atanf(x: REAL): REAL; EXTERN;
PROCEDURE rt_realf(x: INTEGER): REAL; EXTERN;
PROCEDURE rt_entierf(x: REAL): INTEGER; EXTERN;

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
    RETURN rt_realf(x)
END real;

(** Computes the largest integer value not greater than x *)
PROCEDURE entier*(x: REAL): INTEGER;
BEGIN
    RETURN rt_entierf(x)
END entier;

END Math.
